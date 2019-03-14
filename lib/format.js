export function percent(perc) {
    return `${perc.toFixed(1).replace(/\.0$/, '')}%`;
}
