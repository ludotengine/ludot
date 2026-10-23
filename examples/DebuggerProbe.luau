class DebuggerProbe extends Node = {
    private Health: number = 7
    private Cycle: {[string]: any} = {}
    function run(amount: number): number
        self.Cycle.self = self.Cycle
        local total = self.Health + amount
        self.Health = total
        return self.Health
    end
    function runCoroutine(): number
        local thread = coroutine.create(function()
            local amount = 3
            self.Health += amount
            return self.Health
        end)
        local success, result = coroutine.resume(thread)
        assert(success)
        return result
    end
}
