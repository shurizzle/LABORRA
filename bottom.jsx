import React from 'react/cjs/react.production.min.js';
import { css, styled } from 'uebersicht';
import spotify from './lib/spotify';
import { used } from './lib/storage';
import mem from './lib/mem';
import cpu from './lib/cpu';
import * as format from './lib/format';

import Meter from './lib/meter';
import Bar, { Left, Center, Right } from './lib/bar';
import ChildrenWithSeparator from './lib/children-with-separator';

import SpotifyIcon from './lib/icons/spotify';
import RAMIcon from './lib/icons/ram';
import CPUIcon from './lib/icons/cpu';
import FolderIcon from './lib/icons/folder';

const SIZE = 12;
const PADDING = 5;

const TIDS = {};

const iconClass = css({
    marginRight: PADDING,
    color: '#069',
});

const meterClass = css({
    height: SIZE,
    width: `calc(100% - ${SIZE + PADDING * 2}px - 75%)`,
    boxSizing: 'border-box',
    border: '1px solid #069',
    marginLeft: PADDING,

    '& > div': {
        backgroundColor: '#069',
    },
});

const Root = styled('div')({
    position: 'relative',
    height: SIZE,
});

const Track = styled('marquee')({
    display: 'inline-block',
    width: `calc(100% - ${SIZE + PADDING * 2}px - 25%)`,
    whiteSpace: 'nowrap',
    overflowX: '-webkit-marquee',
    WebkitMarqueeDirection: 'backwards',
    WebkitMarqueeIncrement: `6px`,
    WebkitMarqueeRepetition: 'infinite',
    WebkitMarqueeSpeed: 'slow',
    WebkitMarqueeStyle: 'scroll',
});

export const refreshFrequency = false;

export const initialState = {
    spotify: null,
    storage: {},
    mem: 0,
    cpu: [],
};

function updateDiskAction(name, usage) {
    return {
        type: 'DISK_UPDATE',
        name,
        usage,
    };
}

function updateMem(dispatch) {
    const fn = () => {
        mem().then(m => dispatch({ type: 'MEM_UPDATE', data: m }))
            .catch(()=>{})
            .then(() => TIDS.mem = setTimeout(fn, 5000));
    };

    return fn;
}

function updateStorage(dispatch) {
    // const fn = () => {
    //     used('/').then(usage => dispatch(updateDiskAction('root', usage)))
    //         .catch(() => dispatch(updateDiskAction('root', null)))
    //         .then(() => {
    //             used('72920524-97C6-3751-BB67-B400F17DF049')
    //                 .then(usage => dispatch(updateDiskAction('ext', usage)))
    //                 .catch(() => dispatch(updateDiskAction('ext', null)))
    //                 .then(() => TIDS.storage = setTimeout(fn, 10000));
    //         });
    // };

    const fn = () => {
        used('/').then(usage => dispatch(updateDiskAction('root', usage)))
            .catch(() => dispatch(updateDiskAction('root', null)))
            .then(() => TIDS.storage = setTimeout(fn, 10000));
    };

    return fn;
}

function updateCpu(dispatch) {
    const fn = () => {
        cpu().then(data => dispatch({ type: 'CPU_UPDATE', data }))
            .catch(()=>{})
            .then(() => TIDS.cpu = setTimeout(fn, 2000));
    };

    return fn;
}

export const init = (dispatch) => {
    if (TIDS.storage) {
        clearTimeout(TIDS.storage);
    }

    if (TIDS.spotify) {
        clearInterval(TIDS.spotify);
    }

    if (TIDS.mem) {
        clearTimeout(TIDS.mem);
    }

    if (TIDS.cpu) {
        clearTimeout(TIDS.cpu);
    }

    TIDS.spotify = setInterval(() => {
        spotify().then(data => dispatch({type: 'SPOTIFY_UPDATE', data})).
            catch(() => dispatch({type: 'SPOTIFY_UPDATE', data: null}));
    }, 1000);

    updateStorage(dispatch)();
    updateMem(dispatch)();
    updateCpu(dispatch)();
};

export const className = {
    fontFamily: 'Menlo, Consolas, DejaVu Sans Mono, monospace',
    lineHeight: `${SIZE}px`,
    fontSize: SIZE,
    color: '#abb2bf',
    boxSizing: 'border-box',
    bottom: 0,
    left: 0,
    width: '100%',
    padding: PADDING,
    height: PADDING * 2 + SIZE,
    backgroundColor: '#333',
};

const Separator = styled('div')({
    marginLeft: PADDING * 2,
    marginRight: PADDING * 2,
    display: 'inline-block',
    width: 1,
    height: '1em',
    backgroundColor: '#069',
    verticalAlign: 'top',
});

const Spotify = props => {
    if (props && (props.state === 'playing' || props.state === 'paused')) {
        const track = `${props.artist} - ${props.name}`;
        return (
            <React.Fragment>
                <SpotifyIcon className={iconClass} />
                <Track>{track}</Track>
                <Meter
                    className={meterClass}
                    min={0}
                    max={props.duration}
                    value={props.position} />
            </React.Fragment>
        );
    }

    return null;
};

const Cpu = ({ state, ...props }) => {
    if (state.length < 2) {
        return null;
    }

    const usage = cpu.usage(cpu.diff(state[0], state[1]))
        .reduce((a, x) => a + x) / state[1].length;

    return (
        <React.Fragment>
            <CPUIcon className={iconClass} />
            {format.percent(usage)}
        </React.Fragment>
    );
}

export const render = (state) => {
    return (
        <Root>
            <Bar>
                <Left style={{ width: `calc(30% + ${SIZE + PADDING * 2}px)` }}>
                    <Spotify { ...state.spotify } />
                </Left>
                <Center />
                <Right>
                    <ChildrenWithSeparator separator={<Separator />}>
                        <React.Fragment>
                            <RAMIcon className={iconClass} />
                            {format.percent(state.mem)}
                        </React.Fragment>
                        { state.cpu.length == 2 && <Cpu state={state.cpu} /> }
                        <React.Fragment>
                            <FolderIcon className={iconClass} />
                            {format.percent(state.storage.root)}
                        </React.Fragment>
                    </ChildrenWithSeparator>
                </Right>
            </Bar>
        </Root>
    );
}

export const updateState = (event, previousState) => {
    if (event.type === 'SPOTIFY_UPDATE') {
        return {
            ...previousState,
            spotify: event.data,
        };
    } else if (event.type === 'DISK_UPDATE') {
        return {
            ...previousState,
            storage: {
                ...previousState.storage,
                [event.name]: event.usage,
            },
        };
    } else if (event.type === 'MEM_UPDATE') {
        return {
            ...previousState,
            mem: event.data,
        };
    } else if (event.type === 'CPU_UPDATE') {
        let cpu = previousState.cpu;
        if (cpu.length > 1) {
            cpu = [cpu[1], event.data];
        } else {
            cpu.push(event.data);
        }
        return {
            ...previousState,
            cpu,
        };
    }

    return previousState;
}
