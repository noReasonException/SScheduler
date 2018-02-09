# SScheduler
<h3>An EDF Real Time Scheduler implementation</h3><br>

The whole idea for this project is taken from <a href="https://www.embedded.com/design/operating-systems/4204929/Real-Time-Linux-Scheduling-Part-1">this article</a> and is a try to recreate this project so to became a fully stable scheduler , with strong documentation and of course Compatible with newer versions of linux kernel<br>

<h3>Some usefull stuff </h3>
<ul>
	<li>The main scheduling class struct(ss_sched_class) , exist in <a href="https://github.com/NoReasonException/SScheduler/blob/1d90b830425c1df3b35d1178191af4fc46922e57/kernel/sched/ss.c#L38">ss.c</a> file</li>
	<li>The scheduler's utill library exist in <a href="https://github.com/NoReasonException/SScheduler/blob/master/kernel/sched/ss.h">ss.h</a> file</li>
	<li>The scheduler's data structure initializers exist in <a href="https://github.com/NoReasonException/SScheduler/blob/master/kernel/sched/ss_init.c">ss_init.c</a>file</li>

	<li>The scheduler's per_cpu runqueue structure(struct ss_rq) is defined inside <a href="https://github.com/NoReasonException/SScheduler/blob/1d90b830425c1df3b35d1178191af4fc46922e57/kernel/sched/sched.h#L552">sched.h</a> file</li>
	<li>The scheduler's per task structure (struct ss_task) exists also in <a href="https://github.com/NoReasonException/SScheduler/blob/1d90b830425c1df3b35d1178191af4fc46922e57/kernel/sched/ss.c#L38">sched.h</a>file</li>
	<li>The scheduler's configuration constraint is defined in <a href="https://github.com/NoReasonException/SScheduler/blob/1d90b830425c1df3b35d1178191af4fc46922e57/arch/x86/Kconfig#L145">Kconfig</a> file and the scheduling policy is defined in <a href="https://github.com/NoReasonException/SScheduler/blob/1d90b830425c1df3b35d1178191af4fc46922e57/include/uapi/linux/sched.h#L36">include/uapi/sched.h</a> file </li>
</ul>
<h4> the ./updateProject is a simple script to update only the nessary files (so to create a patch later :)  </h4>


