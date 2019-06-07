### Tomasulo算法模拟器

计算机系统结构的课程大作业2

--------------------

能够读入.nel格式的指令文件，解析并运行

具体nel文件格式以及Tomasulo算法的要求见Tomasulo\_tutorial

当前版本不支持分支预测

-------------------

尝试做了一下分支预测，但是失败了
分支预测的主要思路是在每一次jump指令时将当前全局的状态status保存起来，如果分支预测出错了就把这个status恢复
由于可能有多条jump指令一起执行，因此这些status会形成一个栈(在代码中我是用双向链表组织的)



最大的问题在于，每一个status不仅需要保存station,board和register的信息，还要保存这个status栈的信息
否则，我们考虑这样一个场景：

> Jump指令1
>
> Jump指令2

其中Jump1我们预测不跳转，并且它确实不跳转，Jump2我们预测不跳转，但是它事实上会跳转



那么，当Jump1 Issue结束时，status栈的状态为：

> status1

当Jump2 Issue结束时，status栈的状态为：

> status1 -> status2

注意，status2保存的station信息中，Jump1指令处于还没有写回的状态



然后，Jump1指令写回，我们发现预测正确，于是要将status1从栈中remove掉，status栈的状态变为:

> status2

然后，Jump2指令写回，我们发现预测错误，于是通过status2的信息恢复station,board和register,如果status中没有保存status栈的信息，我们将无法对status栈进行恢复，此时status栈的信息变更为

> NULL

但是此时station中记录的信息表示，Jump1指令处于RUNNING状态尚未写回，而当Jump1指令执行到写回阶段时，需要到status栈中查找status1并且删除，此时将会引发重复删除对象的错误



总之很麻烦，也许会有解决办法但是我昨天晚上没睡好而且这个破Tomasulo算法我已经写了差不多一个星期了，不想陪助教玩下去了，不如多花点时间准备期末还有GRE，Iowa也还没有捞出来，SAN值已经不支持我再写下去了



以上