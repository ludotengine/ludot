class BaseActor extends Node2D = {
    export Life: number = 10
    function baseValue(): number
        return self.Life
    end
}
return BaseActor
