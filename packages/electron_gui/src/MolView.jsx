import React, { useRef, useEffect, useContext } from 'react';
import ReactDOM from 'react-dom';
import styles from './MolView.css';
import { CueMolMgr } from './cuemol_system';
import { MgrContext } from "./App.jsx";

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
  canvas.style.width = `${width}px`;
  canvas.style.height = `${height}px`;
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
  const { mgrRef } = useContext(MgrContext);

  console.log('MgrContext:', MgrContext);
  console.log('MolView mgrRef:', mgrRef);

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
  
  useEffect(() => {
    mgr.bindCanvas(canvasRef.current);
    mgr.loadTestPDB(mgr._view.getScene(), mgr._view);
    mgr.updateDisplay();
    mgrRef.current = mgr;
  }, []);

  return (
    <div className={styles.place} ref={placeRef}>
      <canvas className={styles.molView} ref={canvasRef} />
    </div>
  );
}
