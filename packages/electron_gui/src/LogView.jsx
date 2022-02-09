import React, { useRef, useEffect, useState } from 'react';
import styles from './LogView.css';
import { useLogEvent } from './hooks/useLogEvent';

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
