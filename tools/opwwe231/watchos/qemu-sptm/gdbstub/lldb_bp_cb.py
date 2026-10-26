import lldb
def bp_cb(frame, bp_loc, dict):
    thread = frame.GetThread()
    print("[BP HIT]")
    for reg_name in ["x0","x1","x2","x3","x4","x5","x6","x7","x19","x20","x21","x22","x29","x30","sp","pc"]:
        reg = frame.FindRegister(reg_name)
        if reg.IsValid():
            print(f"  {reg_name} = 0x{reg.GetValueAsUnsigned():016x}")
    # dump memory at X3 (likely vp arg) and X0 (likely a1 proc)
    for reg in ["x0","x3","x19","x20"]:
        r = frame.FindRegister(reg)
        if r.IsValid():
            addr = r.GetValueAsUnsigned()
            if addr > 0x100000:
                err = lldb.SBError()
                data = frame.GetThread().GetProcess().ReadMemory(addr, 256, err)
                if err.Success():
                    print(f"[mem @{reg}=0x{addr:x}]: {data.hex()}")
    return False  # don't stop, keep running
