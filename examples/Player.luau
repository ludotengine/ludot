import (add as plus) from "@helpers/mathutils"

class Player extends CharacterBody2D = {
    export Speed: number = 1 in 0 to 1 step 0.1
    private Health: number = 100
    private Values: {number} = {1}

    function _ready()
        print("Ready!", plus(2, 3))
    end

    function pushValue(value: number)
        table.insert(self.Values, value)
    end

    function valueCount(): number
        return #self.Values
    end

    function addNumbers(a: number, b: number): number
        return plus(a, b)
    end

    function damage(amount: number)
        self.Health -= amount
    end
}
