import React, { useRef, useEffect, useState } from 'react';
import ReactDOM from 'react-dom';
import styles from './LogView.css';
const { cuemol, event_manager } = window.myAPI;

function useLogEvent(callback) {
  const handler = ({obj}) => {
    let msg = obj.content;
    // console.log('log event called:', msg);
	if (obj.newline)
	  msg += "\n";
    callback(msg);
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
    callback(accumMsg);

    return () => {
	  event_manager.removeListener(cbid);
    };
  }, []);
}

//////////

export function LogView() {
  const [contents, setContents] = useState('');

  const preRef = useRef(null);
  
  useLogEvent( (msg) => {
    // console.log('log event: contents=', contents);
    // console.log('log event: msg=', msg);
    setContents((contents) => contents + msg);
  });

  useEffect( () => {
    const h = preRef.current.scrollHeight;
	preRef.current.scrollTo(0, h);
    // console.log('scroll to:', h);
  }, [contents]);

  return (
    <div className={styles.bottomContainer}>
      <pre className={styles.logContainer} ref={preRef}>
        {contents}
      </pre>
    </div>
  );
}
