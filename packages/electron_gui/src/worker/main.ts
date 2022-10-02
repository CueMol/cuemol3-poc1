import { WorkerService } from './services';
console.log('worker thread launched');

const svc = new WorkerService();

onmessage = (event: any) : void => {
  const method: string = event.data[0];
  const seqno: number = event.data[1];
  const args: any[] = event.data.slice(2);
  svc.invoke(method, seqno, args);
};
