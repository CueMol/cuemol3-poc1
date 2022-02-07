import React, { useRef, useLayoutEffect } from 'react';
import styles from './MolView.css';
import { useMolView } from './hooks/useMolView.jsx';
import { useCueMol } from './hooks/useCueMol.jsx';
import { cuemol_worker } from './cuemol_worker';

export function MolView() {
  const canvasRef = useRef(null);
  // const placeRef = useRef(null);
  const { molViewID, setMolViewID } = useMolView();
  const { cueMolReady } = useCueMol();

  useLayoutEffect(() => {
    console.log('cuemol ready:', cueMolReady);
    if (cueMolReady) {
      ( async () => {
        const [scene_id, view_id] = await cuemol_worker.createScene();
        console.log('create scene: ', scene_id, view_id);
        const dpr = window.devicePixelRatio || 1;
        await cuemol_worker.bindCanvas(canvasRef.current, view_id, dpr);
        setMolViewID(view_id);
        console.log('useLayoutEffect setMolViewID', molViewID, setMolViewID);

        // TODO: move to elsewhere??
        await cuemol_worker.loadTestPDB(scene_id, view_id);
      })();

    }
  }, [cueMolReady]);

  useLayoutEffect(() => {
    const dpr = window.devicePixelRatio || 1;
    const resizeObserver = new ResizeObserver((_) => {
      if (molViewID !== null) {
        let { width, height } = canvasRef.current.getBoundingClientRect();
        console.log('canvas size:', height, width);
        cuemol_worker.resized(molViewID, width, height, dpr);
      }
    });
    resizeObserver.observe(canvasRef.current);
    return () => {
      resizeObserver.disconnect();
    };
  }, [molViewID]);

  useLayoutEffect(() => {
    const onMouseDown = (event) => {
      if (molViewID !== null) {
        cuemol_worker.onMouseEvent(molViewID, 'mouse-down', event);
      }
    };
    const onMouseUp = (event) => {
      if (molViewID !== null) {
        cuemol_worker.onMouseEvent(molViewID, 'mouse-up', event);
      }
    };
    const onMouseMove = (event) => {
      if (molViewID !== null) {
        cuemol_worker.onMouseEvent(molViewID, 'mouse-move', event);
      }
    };
    const canvas = canvasRef.current;
    canvas.addEventListener('mousedown', onMouseDown);
    canvas.addEventListener('mouseup', onMouseUp);
    canvas.addEventListener('mousemove', onMouseMove);
    return () => {
      canvas.removeEventListener('mousedown', onMouseDown);
      canvas.removeEventListener('mouseup', onMouseUp);
      canvas.removeEventListener('mousemove', onMouseMove);
    };
  }, [molViewID]);


  return (
    <canvas className={styles.molView} ref={canvasRef} />
  );
}
