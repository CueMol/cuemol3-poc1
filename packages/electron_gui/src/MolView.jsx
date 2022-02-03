import React, { useRef, useEffect, useLayoutEffect } from 'react';
import styles from './MolView.css';
import { useMolView } from './use_molview.jsx';
import { cuemol_worker } from './worker_utils';

// function drawArc(canvas) {
//   var ctx = canvas.getContext('2d');
//   ctx.beginPath();
//   ctx.arc(100, 100, 90, 0, Math.PI * 2, true);
//   ctx.stroke();
// }

export function MolView() {
  const canvasRef = useRef(null);
  const placeRef = useRef(null);
  const { molViewID, setMolViewID, cueMolReady } = useMolView();

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
        let { width, height } = placeRef.current.getBoundingClientRect();
        cuemol_worker.resized(molViewID, width, height, dpr);
      }
    });
    resizeObserver.observe(placeRef.current);
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
    <div className={styles.place} ref={placeRef}>
      <canvas className={styles.molView} ref={canvasRef} />
    </div>
  );
}
