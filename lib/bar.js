import { css } from 'uebersicht';
import classNames from 'classnames';

const rootClass = css({
    display: 'flex',
});

function Bar({ className, ...props }) {
    return (
        <div className={classNames(rootClass, className)} { ...props } />
    );
}

export { default as Left } from './bar/left';
export { default as Center } from './bar/center';
export { default as Right } from './bar/right';

export default Bar;
