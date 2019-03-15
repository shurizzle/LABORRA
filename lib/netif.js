import { run } from 'uebersicht';

export default () => run('./lib/netif.cmd').then(JSON.parse);
