import "./BaseActor" as BaseActor
class DerivedActor extends BaseActor = {
    export Armor: number = 2
    function total(): number
        return self.Life + self.Armor
    end
}
return DerivedActor
