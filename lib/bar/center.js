import { css } from 'uebersicht';
import classNames from 'classnames';

const rootClass = css({
    flex: '1 0 auto',
});

function Center({ className, ...props }) {
    return (
        <div className={classNames(rootClass, className)} { ...props } />
    );
}

export default Center;
