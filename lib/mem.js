import { run } from 'uebersicht';
import sysctl from './sysctl';

export default () => {
    return run('LANG=C vm_stat').then(x => {
        const v = x.trim().split(/\n/).slice(1).reduce((acc, line) => {
            line = line.replace(/\.$/, '');
            const index = line.indexOf(':');
            if (index !== -1) {
                const key = line.slice(0, index).trim();
                const value = line.slice(index + 1).trim();
                acc[key] = parseInt(value) * parseInt(sysctl.vm.pagesize);
            }

            return acc;
        }, {});

        return (sysctl.hw.memsize - v['Pages free'] - v['Pages inactive'] - v['Pages speculative']) /
            sysctl.hw.memsize * 100;
    });
};
