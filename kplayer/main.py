import sounddevice
import soundfile
from datetime import datetime
import queue
import threading

DATA_TYPE = "int16"
BLOCKSIZE  = 2048
BUFFERSIZE = 20

def logPrint(*args):
    strTimeStamp = "[" + str(datetime.now()) + "]"
    print(strTimeStamp, *args, flush = True)
    return

def playWavOnDevice(utterDeviceNum, strWavFilePath):
    logPrint(f"    Playing file:'{strWavFilePath}' on device number:{utterDeviceNum}...")
    # Local queue recreated each time
    q = queue.Queue(maxsize=BUFFERSIZE)
    event = threading.Event()

    # Local nested closure function
    def callback(outdata, frames, time, status):
        assert frames == BLOCKSIZE
        if status.output_underflow:
            logPrint("playWavOnDevice:callback: ERROR Output underflow: increase blocksize?")
            raise sounddevice.CallbackAbort
        assert not status
        try:
            logPrint("KKK q.get_nowait()")
            data = q.get_nowait()
            logPrint("KKK q.get_nowait() done")
        except queue.Empty:
            logPrint("playWavOnDevice:callback: ERROR Buffer is empty: increase buffersize?")
            raise sounddevice.CallbackAbort
        if len(data) < len(outdata):
            # Last block
            outdata[:len(data)] = data
            outdata[len(data):] = b'\x00' * (len(outdata) - len(data))
            raise sounddevice.CallbackStop
        else:
            outdata[:] = data

    # Actual code to initiate & control playback
    try:
        with soundfile.SoundFile(strWavFilePath) as f:
            for _ in range(BUFFERSIZE):
                data = f.buffer_read(BLOCKSIZE, dtype=DATA_TYPE)
                if not data:
                    break
                q.put_nowait(data)  # Pre-fill queue
            stream = sounddevice.RawOutputStream(
                samplerate=f.samplerate, blocksize=BLOCKSIZE, channels=f.channels,
                device=utterDeviceNum,  dtype=DATA_TYPE,
                callback=callback, finished_callback=event.set)
            with stream:
                timeout = BLOCKSIZE * BUFFERSIZE / f.samplerate
                while data:
                    data = f.buffer_read(BLOCKSIZE, dtype=DATA_TYPE)
                    logPrint("KKK q.put()")
                    q.put(data, timeout=timeout)
                    logPrint("KKK q.put() done")
                event.wait()  # Wait until playback is finished
    except KeyboardInterrupt:
        # TODO: This does not appear to be hit on a Ctrl-C
        logPrint("playWavOnDevice: ERROR: Keyboard interrupt received")
    except queue.Full:
        # A timeout occurred, i.e. there was an error in the callback
        logPrint("playWavOnDevice: ERROR: Queue timeout")
    except Exception as e:
        logPrint("playWavOnDevice: ERROR: " + type(e).__name__ + ': ' + str(e))


if __name__ == "__main__":
    playWavOnDevice(1, "/Users/kuwang/Music/keai.wav")
    logPrint("Exiting test script...")
