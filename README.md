# Simple stair (and movement) detection for Android

<img src="/screenshot.png" alt="Simple stair detection" width="400px"/>

This app demonstrates a simple approach for activity recognition without any Machine learning involved. The app requires an accelerometer sensor as well as an barometer sensor in order to function!

It is able to detect _movement_ (like walking) as well as stair climbing.

For movement detection the following algorithm is used:
1. Get linear acceleration vector
2. Calculate the length of the acceleration vector: vectorLength = √(accX² + accY² + accZ²)
3. Calculate the EWMA of this length: ewma(i) = 𝛼 * vectorLength + (1 - 𝛼) * ewma(i-1)
4. Check the EWMA for a certain threshold

For stairs detection the following algorithm is used:
1. Get pressure data
    - Remember the initial pressure (only on 1st call)
2. Calculate the EWMA of this data
3. Check whether the device is moving (movement detection algorithm from before)
    - Subtract the EWMA from the remembered pressure data
    - Take its absolute value and check it for a certain threshold
    - Save the EWMA for the next threshold comparison
