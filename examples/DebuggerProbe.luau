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
    function profiled(): number
        return self:getChildCount() + self:profiledHelper()
    end
    function profiledHelper(): number
        local function worker(): number
            coroutine.yield(1)
            return 2
        end
        local thread = coroutine.create(worker)
        local _, first = coroutine.resume(thread)
        local _, second = coroutine.resume(thread)
        local function guarded(): number
            return 0
        end
        local _, zero = pcall(guarded)
        return self.Health + first + second - 3 + zero
    end
    function threaded(amount: number): number
        local doubled = amount * 2
        return doubled
    end
    function startTasks()
        task.delay(1000, function() end)
        task.spawn(function()
            while true do
                task.wait()
                self:taskWorker()
            end
        end)
    end
    function taskWorker(): number
        return self.Health
    end
}
