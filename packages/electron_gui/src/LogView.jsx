import React, { useRef, useEffect } from 'react';
import ReactDOM from 'react-dom';
import styles from './LogView.css';

export function LogView() {
  const preRef = useRef(null);
  
  const handler = function ({obj}) {
    let msg = obj.content;
    console.log('log event called:', msg);
	if (obj.newline)
	  msg += "\n";
    if (preRef.current) {
      preRef.current.appendChild(new Text(msg));
    }
  };

  const evtmgr = window.myAPI.event_manager;


  useEffect(() => {
    const cbid = evtmgr.addListener("log",
				                    -1, //evtmgr.SEM_ANY, // source type
				                    -1, // evtmgr.SEM_ANY, // event type
				                    -1, // evtmgr.SEM_ANY, // source uid
				                    handler);
    return () => {
	  evtmgr.removeListener(cbid);
    };
  }, []);

  return (
    <div className={styles.bottomContainer}>
      <pre className={styles.logContainer} ref={preRef}/>
    </div>
  );
}
