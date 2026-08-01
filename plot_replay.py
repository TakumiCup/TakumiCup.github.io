import csv
import matplotlib.pyplot as plt

t, tan_phi, tilt_dir, axis_h0, ripple = [], [], [], [], []
walls = [[] for _ in range(24)]
with open("replay_snapshots.csv") as f:
    r = csv.reader(f)
    header = next(r)
    for row in r:
        t.append(float(row[0]))
        tan_phi.append(float(row[1]))
        tilt_dir.append(float(row[2]))
        axis_h0.append(float(row[3]))
        ripple.append(float(row[4]))
        for j in range(24):
            walls[j].append(float(row[5 + j]))

fig, axs = plt.subplots(3, 1, figsize=(10, 9), sharex=True)

ax1 = axs[0]
ax1.plot(t, tan_phi, color="tab:blue", label="tan(phi) -- tilt magnitude")
ax1.axhline(0.2, color="gray", linestyle=":", linewidth=1, label="0.2g target")
ax1.set_ylabel("tan(phi)")
ax1.set_title("Tilt response to 0.2g brake pulse (g-force-only oscillator model)")
ax1.legend(loc="upper right", fontsize=8)
ax1.grid(alpha=0.3)

ax2 = axs[1]
# wall_0 = azimuth theta=0 (a fixed compass point on the cup); as tilt
# direction rotates this trace shows the surface climbing/dropping there.
ax2.plot(t, walls[0], color="tab:red", label="wall height @ theta=0 (climbing side)")
ax2.plot(t, walls[12], color="tab:green", label="wall height @ theta=pi (opposite side)")
ax2.plot(t, axis_h0, color="gray", linestyle="--", linewidth=1, label="axis h0")
ax2.set_ylabel("height (mm)")
ax2.legend(loc="upper right", fontsize=8)
ax2.grid(alpha=0.3)

ax3 = axs[2]
ax3.plot(t, ripple, color="tab:purple", label="cosmetic ripple amplitude (mm)")
ax3.set_ylabel("ripple (mm)")
ax3.set_xlabel("time (s)")
ax3.legend(loc="upper right", fontsize=8)
ax3.grid(alpha=0.3)

for ax in axs:
    ax.axvspan(1.0, 1.3, color="orange", alpha=0.15)
    ax.axvspan(1.3, 2.5, color="red", alpha=0.15)
    ax.axvspan(2.5, 2.8, color="orange", alpha=0.15)

plt.tight_layout()
plt.savefig("replay_plot.png", dpi=130)
print("saved replay_plot.png")
