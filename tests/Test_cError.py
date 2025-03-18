import regarch_wrapper
import sys

def test_cError():
    print("=== Testing cError ===")
    try:
        # Attempt to create a cError instance with a test message.
        # Depending on your implementation, this may trigger a program exit.
        err = regarch_wrapper.cError("Test error message from cError")
        print("cError instance created:", err)
    except SystemExit as e:
        # If the constructor calls exit(), we can catch the SystemExit exception.
        print("cError triggered a system exit with exit code:", e.code)
    except Exception as e:
        print("cError raised an unexpected exception:", e)

if __name__ == "__main__":
    test_cError()
