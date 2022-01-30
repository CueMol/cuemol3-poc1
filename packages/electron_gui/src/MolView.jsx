import React, { useRef, useEffect, useLayoutEffect } from 'react';
import styles from './MolView.css';
// import { CueMolMgr } from './cuemol_system';
import { useMolView } from './use_molview.jsx';
import { cuemol_worker } from './worker_utils';

// const mgr = new CueMolMgr(window.myAPI);
// console.log('CueMolMgr instance:', mgr);

function adjustCanvasSize(canvasRef, placeRef, dpr) {
  const canvas = canvasRef.current;
  const place = placeRef.current;
  if (canvas===null ||place===null) {
    return null;
  }
  let { width, height } = place.getBoundingClientRect();
  console.log(`canvas resize: ${width} x ${height}`);
  console.log(`canvas : ${canvas.width} x ${canvas.height}`);
  // canvas.style.top = '0px';
  // canvas.style.left = '0px';
  // canvas.style.width = `${width}px`;
  // canvas.style.height = `${height}px`;
  // canvas.width = width * dpr;
  // canvas.height = height * dpr;
  // mgr.resized(width, height);
  return [width, height];
}

function drawArc(canvas) {
  var ctx = canvas.getContext('2d');
  ctx.beginPath();
  ctx.arc(100, 100, 90, 0, Math.PI * 2, true);
  ctx.stroke();
}

export function MolView() {
  const canvasRef = useRef(null);
  const placeRef = useRef(null);
  const { molViewID, setMolViewID, cueMolReady } = useMolView();

  // useEffect(() => {
  //   const dpr = window.devicePixelRatio || 1;
  //   const resizeObserver = new ResizeObserver((entries) => {
  //     adjustCanvasSize(mgr, canvasRef, placeRef, dpr);
  //     // canvasRef.current && drawArc(canvasRef.current);
  //     mgr.updateDisplay();
  //   });
  //   placeRef.current && resizeObserver.observe(placeRef.current);
  //   adjustCanvasSize(mgr, canvasRef, placeRef, dpr);
  //   mgr.updateDisplay();
  //   return () => {
  //     resizeObserver.disconnect();
  //   };
  // }, []);
  
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

        const resizeObserver = new ResizeObserver((entries) => {
          const [w, h] = adjustCanvasSize(canvasRef, placeRef, dpr);
          // canvasRef.current && drawArc(canvasRef.current);
          cuemol_worker.resized(view_id, w, h, dpr);
        });
        placeRef.current && resizeObserver.observe(placeRef.current);
        const [w, h] = adjustCanvasSize(canvasRef, placeRef, dpr);
        cuemol_worker.resized(view_id, w, h, dpr);

        // TODO: move to elsewhere??
        await cuemol_worker.loadTestPDB(scene_id, view_id);
      })();

      // return () => {
      //   resizeObserver.disconnect();
      // };
    }
  }, [cueMolReady]);

  return (
    <div className={styles.place} ref={placeRef}>
      <canvas className={styles.molView} ref={canvasRef} />
    </div>
  );
}
