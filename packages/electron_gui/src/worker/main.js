import { WorkerService } from './services';
console.log('worker thread launched');

const svc = new WorkerService();

onmessage = (event) => {
  svc.invoke(event.data);
};
