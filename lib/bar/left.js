import { css } from 'uebersicht';
import classNames from 'classnames';

const rootClass = css({
    flex: '0 0 auto',
});

function Left({ className, ...props }) {
    return (
        <div className={classNames(rootClass, className)} { ...props } />
    );
}

export default Left;
