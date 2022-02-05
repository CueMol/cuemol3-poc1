import { useEffect } from 'react';
import * as event from '../event';
import { cuemol_worker } from '../cuemol_worker';
import { useMolView } from '../use_molview.jsx';

export function useLogEvent(callback) {
  const { cueMolReady } = useMolView();

  const handler = ({obj}) => {
    let msg = obj.content;
    console.log('log event called:', msg);
	if (obj.newline)
	  msg += "\n";
    callback(msg);
  };

  useEffect(() => {
    if (cueMolReady) {
      let cbid = null;
      ( async () => {
        const result = await cuemol_worker.addEventListener(
          "log",
	      event.SEM_ANY, // source type
	      event.SEM_ANY, // event type
	      event.SEM_ANY, // source uid
	      handler);
        cbid = result[0];
        console.log('add cuemol event listener cbid=', cbid);
        const accumMsg = await cuemol_worker.startLogger();
        console.log('accumMsg=', accumMsg);
        callback(accumMsg);
      })();
        
      return () => {
	    cuemol_worker.removeEventListener(cbid);
      };
    }
    else {
      return () => {};
    }
  }, [cueMolReady]);
}
