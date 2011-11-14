#!/usr/bin/env python

# A bouncing ball sound
from visual import *
import marsyas
import marsyas_util

# This is a MarSystem that should output the sound of a bouncing ball.
# It will work iteractivelly

# Network:
# sine oscillator => gain => output
spec = ["Series/net", ["SineSource/src1", "Gain/gain","AudioSink/dest"]];
net=marsyas_util.create(spec)

fs = 44100.0
insamples = 256
net.updControl("mrs_real/israte", fs);
net.updControl("mrs_natural/inSamples", 256);
net.updControl("SineSource/src1/mrs_real/frequency", marsyas.MarControlPtr.from_real(440.0));
net.updControl("Gain/gain/mrs_real/gain", marsyas.MarControlPtr.from_real(0.0));
net.updControl("AudioSink/dest/mrs_bool/initAudio", marsyas.MarControlPtr.from_bool(True))

# The bouncing ball algorithm works like this:
#
# The ball is dropped from a certain height H, and falls to the ground by the laws of physics (considering a given g)
# Then, the gain is set to some alpha value, proportional to the ball's hitting speed, and fades exponentially
# In each hit, the bouncing speed is a factor of the collision speed
# If the collision speed is too small, the ball stops.

# These are the parameters for the sphere:
r=2 # Sphere radius
H=10 # Initial height (m)
energy_loss = 0.9 # energy loss in  each collision
g = -9.8 # Gravity acceleration
alpha = 0.1 # Converts velocity to gain
decay = 0.9 # Decay factor for the gain

floor = box (pos=(0,0,0), length=4, height=0.5, width=4, color=color.blue)
ball = sphere (pos=(0,H,0), radius=1, color=color.red)
ball.velocity = vector(0,0,0)

dt = insamples/fs;
currgain = 0.0;
while 1:
	net.tick()
	ball.pos = ball.pos + ball.velocity*dt
	print ball.pos.y, currgain
	if ball.y < ball.radius:
		ball.velocity.y = abs(ball.velocity.y)*energy_loss
		net.updControl("Gain/gain/mrs_real/gain", ball.velocity.y * alpha);
		currgain = ball.velocity.y * alpha;
	else:
#		currgain = net.getControl("Gain/gain/mrs_real/gain")
		currgain = currgain*decay
		net.updControl("Gain/gain/mrs_real/gain", marsyas.MarControlPtr.from_real(currgain));
		ball.velocity.y = ball.velocity.y + (g*dt);


