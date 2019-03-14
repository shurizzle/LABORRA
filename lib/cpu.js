import { run } from 'uebersicht';

function cpu() {
    return run('./lib/cpu.cmd').then(JSON.parse);
}

cpu.diff = function diff(a, b) {
    const res = [];
    for (let i = 0; i < b.length; ++i) {
        res[i] = {
            system: b[i].system - a[i].system,
            user: b[i].user - a[i].user,
            idle: b[i].idle - a[i].idle,
        };
    }

    return res;
};

cpu.usage = function usage(c) {
    return c.map(x => {
        const used = x.system + x.user;
        const total = used + x.idle;
        return used / total * 100;
    });
};

export default cpu;
