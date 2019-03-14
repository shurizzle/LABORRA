import { run } from 'uebersicht';

export default () => {
    return run('./lib/mem.cmd')
        .then(JSON.parse)
        .then(x => x.used / x.total * 100);
};
