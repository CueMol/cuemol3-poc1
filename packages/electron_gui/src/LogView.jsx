import React, { useRef, useEffect } from 'react';
import ReactDOM from 'react-dom';
import styles from './LogView.css';
const { cuemol, event_manager } = window.myAPI;

export function LogView() {
  const preRef = useRef(null);
  
  const handler = function ({obj}) {
    let msg = obj.content;
    console.log('log event called:', msg);
	if (obj.newline)
	  msg += "\n";
    const pre = preRef.current;
    if (pre) {
      pre.appendChild(new Text(msg));
    }
  };

  useEffect(() => {
    const cbid = event_manager.addListener("log",
				                           -1, //event_manager.SEM_ANY, // source type
				                           -1, // event_manager.SEM_ANY, // event type
				                           -1, // event_manager.SEM_ANY, // source uid
				                           handler);
    
    const logMgr = cuemol.getService("MsgLog");
    const accumMsg = logMgr.getAccumMsg();
    logMgr.removeAccumMsg();
    const pre = preRef.current;
    console.log('accumMsg:', accumMsg);
    pre.appendChild(new Text(accumMsg));

    return () => {
	  event_manager.removeListener(cbid);
    };
  }, []);

  return (
    <div className={styles.bottomContainer}>
      <pre className={styles.logContainer} ref={preRef}/>
    </div>
  );
}
