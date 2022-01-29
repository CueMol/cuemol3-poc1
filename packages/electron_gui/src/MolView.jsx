import React, { useRef, useEffect, useLayoutEffect, useContext } from 'react';
import styles from './MolView.css';
import { CueMolMgr } from './cuemol_system';
import { useMolView } from './use_molview.jsx';

const mgr = new CueMolMgr(window.myAPI);
console.log('CueMolMgr instance:', mgr);

function adjustCanvasSize(mgr, canvasRef, placeRef, dpr) {
  const canvas = canvasRef.current;
  const place = placeRef.current;
  if (canvas===null ||place===null) {
    return;
  }
  let { width, height } = place.getBoundingClientRect();
  console.log(`canvas resize: ${width} x ${height}`);
  console.log(`canvas : ${canvas.width} x ${canvas.height}`);
  // canvas.style.top = '0px';
  // canvas.style.left = '0px';
  // canvas.style.width = `${width}px`;
  // canvas.style.height = `${height}px`;
  canvas.width = width * dpr;
  canvas.height = height * dpr;
  mgr.resized(width, height);
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
//   const { mgrRef } = useContext(MgrContext);
  const { molViewID, setMolViewID } = useMolView();

  useEffect(() => {
    const dpr = window.devicePixelRatio || 1;
    const resizeObserver = new ResizeObserver((entries) => {
      adjustCanvasSize(mgr, canvasRef, placeRef, dpr);
      // canvasRef.current && drawArc(canvasRef.current);
      mgr.updateDisplay();
    });
    placeRef.current && resizeObserver.observe(placeRef.current);
    adjustCanvasSize(mgr, canvasRef, placeRef, dpr);
    mgr.updateDisplay();
    return () => {
      resizeObserver.disconnect();
    };
  }, []);
  
  useLayoutEffect(() => {
    mgr.bindCanvas(canvasRef.current);
    setMolViewID(mgr._view.uid);
    console.log('useLayoutEffect setMolViewID', molViewID, setMolViewID);
    // mgrRef.current = mgr;

    // TODO: move to elsewhere??
    mgr.loadTestPDB(mgr._view.getScene(), mgr._view);
    mgr.updateDisplay();
  }, []);

  return (
    <div className={styles.place} ref={placeRef}>
      <canvas className={styles.molView} ref={canvasRef} />
    </div>
  );
}
