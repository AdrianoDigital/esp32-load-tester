import { useEffect, useRef } from "react";

function useWatchdog(timeoutMs, onTimeout) {
  const timerRef = useRef(null);

  const reset = () => {
    if (timerRef.current) {
      clearTimeout(timerRef.current);
    }

    timerRef.current = setTimeout(() => {
      onTimeout();
    }, timeoutMs);
  };

  useEffect(() => {
    reset(); // start on mount

    return () => {
      if (timerRef.current) {
        clearTimeout(timerRef.current);
      }
    };
  }, [timeoutMs, onTimeout]);

  return reset;
}

export default useWatchdog;