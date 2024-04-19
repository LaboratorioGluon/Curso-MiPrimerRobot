import matplotlib.pyplot as plt
import numpy as np
from matplotlib.widgets import Button, Slider

# Model to interact with
class Model:
    def __init__(self):
        self.reset()

    def reset(self):
        self.position = 0
        self.v = 0
        self.a = 0

    def sim(self, input, dt):
        f1 = 10*(input-self.position)
        f2 = -1*self.v
        #f3 = -1*self.a
        a = (f1+f2)/1
        v = self.v + ((a+self.a)/2)*dt
        p = self.position + ((v+self.v)/2)*dt

        self.v = v
        self.a = a
        self.position = p

        return (p,v,a)

## PID implementation
class PID:
    def __init__(self):
        self.reset()

    def reset(self):
        self.firstrun = True
        self.i = 0
        self.preve = 0


    def calculate(self, e,kp,ki,kd, dt):
        if self.firstrun:
            self.preve = e
            self.firstrun = False

        p = e*kp
        self.i = self.i + ki*(self.preve+e)/2
        d = kd*(e-self.preve)
        self.preve = e
        return p+self.i+d


## Simulation preparation

dt = 0.001
t = np.arange(0,6, dt)
obj = np.heaviside(t-0.1, 1)
pid = PID()

exc = np.heaviside(t-0.1, 1)
out = []


def simulate(kp,ki,kd):
    global t, obj
    m = Model()
    ps = []
    vs = []
    ass = []
    ex = 0
    pid.reset()
    m.reset()
    out = []
    for i in range(len(t)):
            
        p,v,a = m.sim(ex, dt)
        ps.append(p)
        vs.append(v)
        ass.append(a)

        ex = pid.calculate(obj[i]-p, kp,ki,kd,dt)
        if ex > 100:
            ex = 100
        out.append(ex)
    return (ps, vs, ass, out)


## Axis and Sliders

fig, ax = plt.subplots()
fig.subplots_adjust(left=0.1, bottom=0.5)
axkp = fig.add_axes([0.1, 0.1, 0.65, 0.03])
kp_slider = Slider(
    ax=axkp,
    label='Kp',
    valmin=0,
    valmax=200,
    valinit=0,
)

axki = fig.add_axes([0.1, 0.2, 0.65, 0.03])
ki_slider = Slider(
    ax=axki,
    label='Ki',
    valmin=0,
    valmax=0.5,
    valinit=0,
)

axkd = fig.add_axes([0.1, 0.3, 0.65, 0.03])
kd_slider = Slider(
    ax=axkd,
    label='Kd',
    valmin=0,
    valmax=15000,
    valinit=0,
)


ps, vs, ass, ex = simulate(0, 0,0)
ax.plot(t, exc)
line,= ax.plot(t, ps, label="ps")
def update(new_val):
    global line, line2
    ps, vs, ass, ex = simulate(kp_slider.val, ki_slider.val,kd_slider.val)
    line.set_ydata(ps)
    fig.canvas.draw_idle()
    ax.set_ylim(0-0.1, max([max(obj)*1.1, max(ps)*1.1]))


kp_slider.on_changed(update)
ki_slider.on_changed(update)
kd_slider.on_changed(update)

ax.legend()
plt.show()

