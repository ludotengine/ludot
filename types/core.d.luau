-- Generated from Ludot reflection. Run tools/generate_api_types.py to update.

declare extern type Vector2 with
    X: number
    Y: number
    function angle(self): number
    function angleTo(self, arg0: Vector2): number
    function angleToPoint(self, arg0: Vector2): number
    function directionTo(self, arg0: Vector2): Vector2
    function distanceTo(self, arg0: Vector2): number
    function distanceSquaredTo(self, arg0: Vector2): number
    function length(self): number
    function lengthSquared(self): number
    function limitLength(self, arg0: (number)?): Vector2
    function normalized(self): Vector2
    function isNormalized(self): boolean
    function isEqualApprox(self, arg0: Vector2): boolean
    function isZeroApprox(self): boolean
    function isFinite(self): boolean
    function posmod(self, arg0: number): Vector2
    function posmodv(self, arg0: Vector2): Vector2
    function project(self, arg0: Vector2): Vector2
    function lerp(self, arg0: Vector2, arg1: number): Vector2
    function slerp(self, arg0: Vector2, arg1: number): Vector2
    function cubicInterpolate(self, arg0: Vector2, arg1: Vector2, arg2: Vector2, arg3: number): Vector2
    function cubicInterpolateInTime(self, arg0: Vector2, arg1: Vector2, arg2: Vector2, arg3: number, arg4: number, arg5: number, arg6: number): Vector2
    function bezierInterpolate(self, arg0: Vector2, arg1: Vector2, arg2: Vector2, arg3: number): Vector2
    function bezierDerivative(self, arg0: Vector2, arg1: Vector2, arg2: Vector2, arg3: number): Vector2
    function maxAxisIndex(self): number
    function minAxisIndex(self): number
    function moveToward(self, arg0: Vector2, arg1: number): Vector2
    function rotated(self, arg0: number): Vector2
    function orthogonal(self): Vector2
    function floor(self): Vector2
    function ceil(self): Vector2
    function round(self): Vector2
    function aspect(self): number
    function dot(self, arg0: Vector2): number
    function slide(self, arg0: Vector2): Vector2
    function bounce(self, arg0: Vector2): Vector2
    function reflect(self, arg0: Vector2): Vector2
    function cross(self, arg0: Vector2): number
    function abs(self): Vector2
    function sign(self): Vector2
    function clamp(self, arg0: Vector2, arg1: Vector2): Vector2
    function clampf(self, arg0: number, arg1: number): Vector2
    function snapped(self, arg0: Vector2): Vector2
    function snappedf(self, arg0: number): Vector2
    function min(self, arg0: Vector2): Vector2
    function minf(self, arg0: number): Vector2
    function max(self, arg0: Vector2): Vector2
    function maxf(self, arg0: number): Vector2
    function fromAngle(self, arg0: number): Vector2
end

declare extern type Vector2i with
    X: number
    Y: number
    function aspect(self): number
    function maxAxisIndex(self): number
    function minAxisIndex(self): number
    function distanceTo(self, arg0: Vector2i): number
    function distanceSquaredTo(self, arg0: Vector2i): number
    function length(self): number
    function lengthSquared(self): number
    function sign(self): Vector2i
    function abs(self): Vector2i
    function clamp(self, arg0: Vector2i, arg1: Vector2i): Vector2i
    function clampi(self, arg0: number, arg1: number): Vector2i
    function snapped(self, arg0: Vector2i): Vector2i
    function snappedi(self, arg0: number): Vector2i
    function min(self, arg0: Vector2i): Vector2i
    function mini(self, arg0: number): Vector2i
    function max(self, arg0: Vector2i): Vector2i
    function maxi(self, arg0: number): Vector2i
end

declare extern type Rect2 with
    Position: Vector2
    Size: Vector2
    End: Vector2
    function getCenter(self): Vector2
    function getArea(self): number
    function hasArea(self): boolean
    function hasPoint(self, arg0: Vector2): boolean
    function isEqualApprox(self, arg0: Rect2): boolean
    function isFinite(self): boolean
    function intersects(self, arg0: Rect2, arg1: (boolean)?): boolean
    function encloses(self, arg0: Rect2): boolean
    function intersection(self, arg0: Rect2): Rect2
    function merge(self, arg0: Rect2): Rect2
    function expand(self, arg0: Vector2): Rect2
    function getSupport(self, arg0: Vector2): Vector2
    function grow(self, arg0: number): Rect2
    function growSide(self, arg0: number, arg1: number): Rect2
    function growIndividual(self, arg0: number, arg1: number, arg2: number, arg3: number): Rect2
    function abs(self): Rect2
end

declare extern type Rect2i with
    Position: Vector2i
    Size: Vector2i
    End: Vector2i
    function getCenter(self): Vector2i
    function getArea(self): number
    function hasArea(self): boolean
    function hasPoint(self, arg0: Vector2i): boolean
    function intersects(self, arg0: Rect2i): boolean
    function encloses(self, arg0: Rect2i): boolean
    function intersection(self, arg0: Rect2i): Rect2i
    function merge(self, arg0: Rect2i): Rect2i
    function expand(self, arg0: Vector2i): Rect2i
    function grow(self, arg0: number): Rect2i
    function growSide(self, arg0: number, arg1: number): Rect2i
    function growIndividual(self, arg0: number, arg1: number, arg2: number, arg3: number): Rect2i
    function abs(self): Rect2i
end

declare extern type Vector3 with
    X: number
    Y: number
    Z: number
    function minAxisIndex(self): number
    function maxAxisIndex(self): number
    function angleTo(self, arg0: Vector3): number
    function signedAngleTo(self, arg0: Vector3, arg1: Vector3): number
    function directionTo(self, arg0: Vector3): Vector3
    function distanceTo(self, arg0: Vector3): number
    function distanceSquaredTo(self, arg0: Vector3): number
    function length(self): number
    function lengthSquared(self): number
    function limitLength(self, arg0: (number)?): Vector3
    function normalized(self): Vector3
    function isNormalized(self): boolean
    function isEqualApprox(self, arg0: Vector3): boolean
    function isZeroApprox(self): boolean
    function isFinite(self): boolean
    function inverse(self): Vector3
    function clamp(self, arg0: Vector3, arg1: Vector3): Vector3
    function clampf(self, arg0: number, arg1: number): Vector3
    function snapped(self, arg0: Vector3): Vector3
    function snappedf(self, arg0: number): Vector3
    function rotated(self, arg0: Vector3, arg1: number): Vector3
    function lerp(self, arg0: Vector3, arg1: number): Vector3
    function slerp(self, arg0: Vector3, arg1: number): Vector3
    function cubicInterpolate(self, arg0: Vector3, arg1: Vector3, arg2: Vector3, arg3: number): Vector3
    function cubicInterpolateInTime(self, arg0: Vector3, arg1: Vector3, arg2: Vector3, arg3: number, arg4: number, arg5: number, arg6: number): Vector3
    function bezierInterpolate(self, arg0: Vector3, arg1: Vector3, arg2: Vector3, arg3: number): Vector3
    function bezierDerivative(self, arg0: Vector3, arg1: Vector3, arg2: Vector3, arg3: number): Vector3
    function moveToward(self, arg0: Vector3, arg1: number): Vector3
    function dot(self, arg0: Vector3): number
    function cross(self, arg0: Vector3): Vector3
    function outer(self, arg0: Vector3): Basis
    function abs(self): Vector3
    function floor(self): Vector3
    function ceil(self): Vector3
    function round(self): Vector3
    function posmod(self, arg0: number): Vector3
    function posmodv(self, arg0: Vector3): Vector3
    function project(self, arg0: Vector3): Vector3
    function slide(self, arg0: Vector3): Vector3
    function bounce(self, arg0: Vector3): Vector3
    function reflect(self, arg0: Vector3): Vector3
    function sign(self): Vector3
    function octahedronEncode(self): Vector2
    function min(self, arg0: Vector3): Vector3
    function minf(self, arg0: number): Vector3
    function max(self, arg0: Vector3): Vector3
    function maxf(self, arg0: number): Vector3
    function octahedronDecode(self, arg0: Vector2): Vector3
end

declare extern type Vector3i with
    X: number
    Y: number
    Z: number
    function minAxisIndex(self): number
    function maxAxisIndex(self): number
    function distanceTo(self, arg0: Vector3i): number
    function distanceSquaredTo(self, arg0: Vector3i): number
    function length(self): number
    function lengthSquared(self): number
    function sign(self): Vector3i
    function abs(self): Vector3i
    function clamp(self, arg0: Vector3i, arg1: Vector3i): Vector3i
    function clampi(self, arg0: number, arg1: number): Vector3i
    function snapped(self, arg0: Vector3i): Vector3i
    function snappedi(self, arg0: number): Vector3i
    function min(self, arg0: Vector3i): Vector3i
    function mini(self, arg0: number): Vector3i
    function max(self, arg0: Vector3i): Vector3i
    function maxi(self, arg0: number): Vector3i
end

declare extern type Transform2D with
    X: Vector2
    Y: Vector2
    Origin: Vector2
    function inverse(self): Transform2D
    function affineInverse(self): Transform2D
    function getRotation(self): number
    function getOrigin(self): Vector2
    function getScale(self): Vector2
    function getSkew(self): number
    function orthonormalized(self): Transform2D
    function rotated(self, arg0: number): Transform2D
    function rotatedLocal(self, arg0: number): Transform2D
    function scaled(self, arg0: Vector2): Transform2D
    function scaledLocal(self, arg0: Vector2): Transform2D
    function translated(self, arg0: Vector2): Transform2D
    function translatedLocal(self, arg0: Vector2): Transform2D
    function determinant(self): number
    function basisXform(self, arg0: Vector2): Vector2
    function basisXformInv(self, arg0: Vector2): Vector2
    function interpolateWith(self, arg0: Transform2D, arg1: number): Transform2D
    function isConformal(self): boolean
    function isEqualApprox(self, arg0: Transform2D): boolean
    function isFinite(self): boolean
    function lookingAt(self, arg0: (Vector2)?): Transform2D
end

declare extern type Vector4 with
    X: number
    Y: number
    Z: number
    W: number
    function minAxisIndex(self): number
    function maxAxisIndex(self): number
    function length(self): number
    function lengthSquared(self): number
    function abs(self): Vector4
    function sign(self): Vector4
    function floor(self): Vector4
    function ceil(self): Vector4
    function round(self): Vector4
    function lerp(self, arg0: Vector4, arg1: number): Vector4
    function cubicInterpolate(self, arg0: Vector4, arg1: Vector4, arg2: Vector4, arg3: number): Vector4
    function cubicInterpolateInTime(self, arg0: Vector4, arg1: Vector4, arg2: Vector4, arg3: number, arg4: number, arg5: number, arg6: number): Vector4
    function posmod(self, arg0: number): Vector4
    function posmodv(self, arg0: Vector4): Vector4
    function snapped(self, arg0: Vector4): Vector4
    function snappedf(self, arg0: number): Vector4
    function clamp(self, arg0: Vector4, arg1: Vector4): Vector4
    function clampf(self, arg0: number, arg1: number): Vector4
    function normalized(self): Vector4
    function isNormalized(self): boolean
    function directionTo(self, arg0: Vector4): Vector4
    function distanceTo(self, arg0: Vector4): number
    function distanceSquaredTo(self, arg0: Vector4): number
    function dot(self, arg0: Vector4): number
    function inverse(self): Vector4
    function isEqualApprox(self, arg0: Vector4): boolean
    function isZeroApprox(self): boolean
    function isFinite(self): boolean
    function min(self, arg0: Vector4): Vector4
    function minf(self, arg0: number): Vector4
    function max(self, arg0: Vector4): Vector4
    function maxf(self, arg0: number): Vector4
end

declare extern type Vector4i with
    X: number
    Y: number
    Z: number
    W: number
    function minAxisIndex(self): number
    function maxAxisIndex(self): number
    function length(self): number
    function lengthSquared(self): number
    function sign(self): Vector4i
    function abs(self): Vector4i
    function clamp(self, arg0: Vector4i, arg1: Vector4i): Vector4i
    function clampi(self, arg0: number, arg1: number): Vector4i
    function snapped(self, arg0: Vector4i): Vector4i
    function snappedi(self, arg0: number): Vector4i
    function min(self, arg0: Vector4i): Vector4i
    function mini(self, arg0: number): Vector4i
    function max(self, arg0: Vector4i): Vector4i
    function maxi(self, arg0: number): Vector4i
    function distanceTo(self, arg0: Vector4i): number
    function distanceSquaredTo(self, arg0: Vector4i): number
end

declare extern type Plane with
    X: number
    Y: number
    Z: number
    D: number
    Normal: Vector3
    function normalized(self): Plane
    function getCenter(self): Vector3
    function isEqualApprox(self, arg0: Plane): boolean
    function isFinite(self): boolean
    function isPointOver(self, arg0: Vector3): boolean
    function distanceTo(self, arg0: Vector3): number
    function hasPoint(self, arg0: Vector3, arg1: (number)?): boolean
    function project(self, arg0: Vector3): Vector3
    function intersect3(self, arg0: Plane, arg1: Plane): any
    function intersectsRay(self, arg0: Vector3, arg1: Vector3): any
    function intersectsSegment(self, arg0: Vector3, arg1: Vector3): any
end

declare extern type Quaternion with
    X: number
    Y: number
    Z: number
    W: number
    function length(self): number
    function lengthSquared(self): number
    function normalized(self): Quaternion
    function isNormalized(self): boolean
    function isEqualApprox(self, arg0: Quaternion): boolean
    function isFinite(self): boolean
    function inverse(self): Quaternion
    function log(self): Quaternion
    function exp(self): Quaternion
    function angleTo(self, arg0: Quaternion): number
    function dot(self, arg0: Quaternion): number
    function slerp(self, arg0: Quaternion, arg1: number): Quaternion
    function slerpni(self, arg0: Quaternion, arg1: number): Quaternion
    function sphericalCubicInterpolate(self, arg0: Quaternion, arg1: Quaternion, arg2: Quaternion, arg3: number): Quaternion
    function sphericalCubicInterpolateInTime(self, arg0: Quaternion, arg1: Quaternion, arg2: Quaternion, arg3: number, arg4: number, arg5: number, arg6: number): Quaternion
    function getEuler(self, arg0: (number)?): Vector3
    function fromEuler(self, arg0: Vector3): Quaternion
    function getAxis(self): Vector3
    function getAngle(self): number
end

declare extern type AABB with
    Position: Vector3
    Size: Vector3
    End: Vector3
    function abs(self): AABB
    function getCenter(self): Vector3
    function getVolume(self): number
    function hasVolume(self): boolean
    function hasSurface(self): boolean
    function hasPoint(self, arg0: Vector3): boolean
    function isEqualApprox(self, arg0: AABB): boolean
    function isFinite(self): boolean
    function intersects(self, arg0: AABB): boolean
    function encloses(self, arg0: AABB): boolean
    function intersectsPlane(self, arg0: Plane): boolean
    function intersection(self, arg0: AABB): AABB
    function merge(self, arg0: AABB): AABB
    function expand(self, arg0: Vector3): AABB
    function grow(self, arg0: number): AABB
    function getSupport(self, arg0: Vector3): Vector3
    function getLongestAxis(self): Vector3
    function getLongestAxisIndex(self): number
    function getLongestAxisSize(self): number
    function getShortestAxis(self): Vector3
    function getShortestAxisIndex(self): number
    function getShortestAxisSize(self): number
    function getEndpoint(self, arg0: number): Vector3
    function intersectsSegment(self, arg0: Vector3, arg1: Vector3): any
    function intersectsRay(self, arg0: Vector3, arg1: Vector3): any
end

declare extern type Basis with
    X: Vector3
    Y: Vector3
    Z: Vector3
    function inverse(self): Basis
    function transposed(self): Basis
    function orthonormalized(self): Basis
    function determinant(self): number
    function rotated(self, arg0: Vector3, arg1: number): Basis
    function scaled(self, arg0: Vector3): Basis
    function scaledLocal(self, arg0: Vector3): Basis
    function getScale(self): Vector3
    function getEuler(self, arg0: (number)?): Vector3
    function tdotx(self, arg0: Vector3): number
    function tdoty(self, arg0: Vector3): number
    function tdotz(self, arg0: Vector3): number
    function slerp(self, arg0: Basis, arg1: number): Basis
    function isConformal(self): boolean
    function isEqualApprox(self, arg0: Basis): boolean
    function isFinite(self): boolean
    function isOrthonormal(self): boolean
    function getRotationQuaternion(self): Quaternion
    function lookingAt(self, arg0: Vector3, arg1: (Vector3)?, arg2: (boolean)?): Basis
    function fromScale(self, arg0: Vector3): Basis
    function fromEuler(self, arg0: Vector3, arg1: (number)?): Basis
end

declare extern type Transform3D with
    Basis: Basis
    Origin: Vector3
    function inverse(self): Transform3D
    function affineInverse(self): Transform3D
    function orthonormalized(self): Transform3D
    function rotated(self, arg0: Vector3, arg1: number): Transform3D
    function rotatedLocal(self, arg0: Vector3, arg1: number): Transform3D
    function scaled(self, arg0: Vector3): Transform3D
    function scaledLocal(self, arg0: Vector3): Transform3D
    function translated(self, arg0: Vector3): Transform3D
    function translatedLocal(self, arg0: Vector3): Transform3D
    function lookingAt(self, arg0: Vector3, arg1: (Vector3)?, arg2: (boolean)?): Transform3D
    function interpolateWith(self, arg0: Transform3D, arg1: number): Transform3D
    function isEqualApprox(self, arg0: Transform3D): boolean
    function isFinite(self): boolean
end

declare extern type Projection with
    X: Vector4
    Y: Vector4
    Z: Vector4
    W: Vector4
    function createDepthCorrection(self, arg0: boolean): Projection
    function createLightAtlasRect(self, arg0: Rect2): Projection
    function createPerspective(self, arg0: number, arg1: number, arg2: number, arg3: number, arg4: (boolean)?): Projection
    function createPerspectiveHmd(self, arg0: number, arg1: number, arg2: number, arg3: number, arg4: boolean, arg5: number, arg6: number, arg7: number): Projection
    function createForHmd(self, arg0: number, arg1: number, arg2: number, arg3: number, arg4: number, arg5: number, arg6: number, arg7: number): Projection
    function createOrthogonal(self, arg0: number, arg1: number, arg2: number, arg3: number, arg4: number, arg5: number): Projection
    function createOrthogonalAspect(self, arg0: number, arg1: number, arg2: number, arg3: number, arg4: (boolean)?): Projection
    function createFrustum(self, arg0: number, arg1: number, arg2: number, arg3: number, arg4: number, arg5: number): Projection
    function createFrustumAspect(self, arg0: number, arg1: number, arg2: Vector2, arg3: number, arg4: number, arg5: (boolean)?): Projection
    function createFitAabb(self, arg0: AABB): Projection
    function determinant(self): number
    function perspectiveZnearAdjusted(self, arg0: number): Projection
    function getProjectionPlane(self, arg0: number): Plane
    function flippedY(self): Projection
    function jitterOffseted(self, arg0: Vector2): Projection
    function getFovy(self, arg0: number, arg1: number): number
    function getZFar(self): number
    function getZNear(self): number
    function getAspect(self): number
    function getFov(self): number
    function isOrthogonal(self): boolean
    function getViewportHalfExtents(self): Vector2
    function getFarPlaneHalfExtents(self): Vector2
    function inverse(self): Projection
    function getPixelsPerMeter(self, arg0: number): number
    function getLodMultiplier(self): number
end

declare extern type Color with
    R: number
    G: number
    B: number
    A: number
    R8: number
    G8: number
    B8: number
    A8: number
    H: number
    S: number
    V: number
    OkHslH: number
    OkHslS: number
    OkHslL: number
    function toArgb32(self): number
    function toAbgr32(self): number
    function toRgba32(self): number
    function toArgb64(self): number
    function toAbgr64(self): number
    function toRgba64(self): number
    function toHtml(self, arg0: (boolean)?): string
    function clamp(self, arg0: (Color)?, arg1: (Color)?): Color
    function inverted(self): Color
    function lerp(self, arg0: Color, arg1: number): Color
    function lightened(self, arg0: number): Color
    function darkened(self, arg0: number): Color
    function blend(self, arg0: Color): Color
    function getLuminance(self): number
    function srgbToLinear(self): Color
    function linearToSrgb(self): Color
    function isEqualApprox(self, arg0: Color): boolean
    function hex(self, arg0: number): Color
    function hex64(self, arg0: number): Color
    function html(self, arg0: string): Color
    function htmlIsValid(self, arg0: string): boolean
    function fromString(self, arg0: string, arg1: Color): Color
    function fromHsv(self, arg0: number, arg1: number, arg2: number, arg3: (number)?): Color
    function fromOkHsl(self, arg0: number, arg1: number, arg2: number, arg3: (number)?): Color
    function fromRgbe9995(self, arg0: number): Color
    function fromRgba8(self, arg0: number, arg1: number, arg2: number, arg3: (number)?): Color
end

declare extern type RID with
    function isValid(self): boolean
    function getId(self): number
end

declare extern type Callable with
    function create(self, arg0: any, arg1: string): Callable
    function callv(self, arg0: Array): any
    function isNull(self): boolean
    function isCustom(self): boolean
    function isStandard(self): boolean
    function isValid(self): boolean
    function getObject(self): Object
    function getObjectId(self): number
    function getMethod(self): string
    function getArgumentCount(self): number
    function getBoundArgumentsCount(self): number
    function getBoundArguments(self): Array
    function getUnboundArgumentsCount(self): number
    function hash(self): number
    function bindv(self, arg0: Array): Callable
    function unbind(self, arg0: number): Callable
    function call(self, ...: any): any
    function callDeferred(self, ...: any): ()
    function rpc(self, ...: any): ()
    function rpcId(self, arg0: number, ...: any): ()
    function bind(self, ...: any): Callable
end

declare extern type Signal with
    function isNull(self): boolean
    function getObject(self): Object
    function getObjectId(self): number
    function getName(self): string
    function connect(self, arg0: Callable, arg1: (number)?): number
    function disconnect(self, arg0: Callable): ()
    function isConnected(self, arg0: Callable): boolean
    function getConnections(self): Array
    function hasConnections(self): boolean
    function emit(self, ...: any): ()
end

declare extern type Dictionary with
    function size(self): number
    function isEmpty(self): boolean
    function clear(self): ()
    function assign(self, arg0: Dictionary): ()
    function sort(self): ()
    function merge(self, arg0: Dictionary, arg1: (boolean)?): ()
    function merged(self, arg0: Dictionary, arg1: (boolean)?): Dictionary
    function has(self, arg0: any): boolean
    function hasAll(self, arg0: Array): boolean
    function findKey(self, arg0: any): any
    function erase(self, arg0: any): boolean
    function hash(self): number
    function keys(self): Array
    function values(self): Array
    function duplicate(self, arg0: (boolean)?): Dictionary
    function duplicateDeep(self, arg0: (number)?): Dictionary
    function get(self, arg0: any, arg1: (any)?): any
    function getOrAdd(self, arg0: any, arg1: (any)?): any
    function set(self, arg0: any, arg1: any): boolean
    function isTyped(self): boolean
    function isTypedKey(self): boolean
    function isTypedValue(self): boolean
    function isSameTyped(self, arg0: Dictionary): boolean
    function isSameTypedKey(self, arg0: Dictionary): boolean
    function isSameTypedValue(self, arg0: Dictionary): boolean
    function getTypedKeyBuiltin(self): number
    function getTypedValueBuiltin(self): number
    function getTypedKeyClassName(self): string
    function getTypedValueClassName(self): string
    function getTypedKeyScript(self): any
    function getTypedValueScript(self): any
    function makeReadOnly(self): ()
    function isReadOnly(self): boolean
    function recursiveEqual(self, arg0: Dictionary, arg1: number): boolean
end

declare extern type Array with
    function size(self): number
    function isEmpty(self): boolean
    function clear(self): ()
    function hash(self): number
    function assign(self, arg0: Array): ()
    function get(self, arg0: number): any
    function set(self, arg0: number, arg1: any): ()
    function pushBack(self, arg0: any): ()
    function pushFront(self, arg0: any): ()
    function append(self, arg0: any): ()
    function appendArray(self, arg0: Array): ()
    function resize(self, arg0: number): number
    function insert(self, arg0: number, arg1: any): number
    function removeAt(self, arg0: number): ()
    function fill(self, arg0: any): ()
    function erase(self, arg0: any): ()
    function front(self): any
    function back(self): any
    function pickRandom(self): any
    function find(self, arg0: any, arg1: (number)?): number
    function findCustom(self, arg0: Callable, arg1: (number)?): number
    function rfind(self, arg0: any, arg1: (number)?): number
    function rfindCustom(self, arg0: Callable, arg1: (number)?): number
    function count(self, arg0: any): number
    function has(self, arg0: any): boolean
    function popBack(self): any
    function popFront(self): any
    function popAt(self, arg0: number): any
    function sort(self): ()
    function sortCustom(self, arg0: Callable): ()
    function shuffle(self): ()
    function bsearch(self, arg0: any, arg1: (boolean)?): number
    function bsearchCustom(self, arg0: any, arg1: Callable, arg2: (boolean)?): number
    function reverse(self): ()
    function duplicate(self, arg0: (boolean)?): Array
    function duplicateDeep(self, arg0: (number)?): Array
    function slice(self, arg0: number, arg1: (number)?, arg2: (number)?, arg3: (boolean)?): Array
    function filter(self, arg0: Callable): Array
    function map(self, arg0: Callable): Array
    function reduce(self, arg0: Callable, arg1: (any)?): any
    function any(self, arg0: Callable): boolean
    function all(self, arg0: Callable): boolean
    function max(self): any
    function min(self): any
    function isTyped(self): boolean
    function isSameTyped(self, arg0: Array): boolean
    function getTypedBuiltin(self): number
    function getTypedClassName(self): string
    function getTypedScript(self): any
    function makeReadOnly(self): ()
    function isReadOnly(self): boolean
end

declare extern type PackedByteArray with
    function get(self, arg0: number): number
    function set(self, arg0: number, arg1: number): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: number): boolean
    function append(self, arg0: number): boolean
    function appendArray(self, arg0: PackedByteArray): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: number): number
    function fill(self, arg0: number): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: number): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: number, arg1: (boolean)?): number
    function duplicate(self): PackedByteArray
    function find(self, arg0: number, arg1: (number)?): number
    function rfind(self, arg0: number, arg1: (number)?): number
    function count(self, arg0: number): number
    function erase(self, arg0: number): boolean
    function getStringFromAscii(self): string
    function getStringFromUtf8(self): string
    function getStringFromUtf16(self): string
    function getStringFromUtf32(self): string
    function getStringFromWchar(self): string
    function getStringFromMultibyteChar(self, arg0: (string)?): string
    function hexEncode(self): string
    function compress(self, arg0: (number)?): PackedByteArray
    function decompress(self, arg0: number, arg1: (number)?): PackedByteArray
    function decompressDynamic(self, arg0: number, arg1: (number)?): PackedByteArray
    function decodeU8(self, arg0: number): number
    function decodeS8(self, arg0: number): number
    function decodeU16(self, arg0: number): number
    function decodeS16(self, arg0: number): number
    function decodeU32(self, arg0: number): number
    function decodeS32(self, arg0: number): number
    function decodeU64(self, arg0: number): number
    function decodeS64(self, arg0: number): number
    function decodeHalf(self, arg0: number): number
    function decodeFloat(self, arg0: number): number
    function decodeDouble(self, arg0: number): number
    function hasEncodedVar(self, arg0: number, arg1: (boolean)?): boolean
    function decodeVar(self, arg0: number, arg1: (boolean)?): any
    function decodeVarSize(self, arg0: number, arg1: (boolean)?): number
    function toInt32Array(self): PackedInt32Array
    function toInt64Array(self): PackedInt64Array
    function toFloat32Array(self): PackedFloat32Array
    function toFloat64Array(self): PackedFloat64Array
    function toVector2Array(self): PackedVector2Array
    function toVector3Array(self): PackedVector3Array
    function toVector4Array(self): PackedVector4Array
    function toColorArray(self): PackedColorArray
    function bswap16(self, arg0: (number)?, arg1: (number)?): ()
    function bswap32(self, arg0: (number)?, arg1: (number)?): ()
    function bswap64(self, arg0: (number)?, arg1: (number)?): ()
    function encodeU8(self, arg0: number, arg1: number): ()
    function encodeS8(self, arg0: number, arg1: number): ()
    function encodeU16(self, arg0: number, arg1: number): ()
    function encodeS16(self, arg0: number, arg1: number): ()
    function encodeU32(self, arg0: number, arg1: number): ()
    function encodeS32(self, arg0: number, arg1: number): ()
    function encodeU64(self, arg0: number, arg1: number): ()
    function encodeS64(self, arg0: number, arg1: number): ()
    function encodeHalf(self, arg0: number, arg1: number): ()
    function encodeFloat(self, arg0: number, arg1: number): ()
    function encodeDouble(self, arg0: number, arg1: number): ()
    function encodeVar(self, arg0: number, arg1: any, arg2: (boolean)?): number
end

declare extern type PackedInt32Array with
    function get(self, arg0: number): number
    function set(self, arg0: number, arg1: number): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: number): boolean
    function append(self, arg0: number): boolean
    function appendArray(self, arg0: PackedInt32Array): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: number): number
    function fill(self, arg0: number): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: number): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedInt32Array
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: number, arg1: (boolean)?): number
    function duplicate(self): PackedInt32Array
    function find(self, arg0: number, arg1: (number)?): number
    function rfind(self, arg0: number, arg1: (number)?): number
    function count(self, arg0: number): number
    function erase(self, arg0: number): boolean
end

declare extern type PackedInt64Array with
    function get(self, arg0: number): number
    function set(self, arg0: number, arg1: number): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: number): boolean
    function append(self, arg0: number): boolean
    function appendArray(self, arg0: PackedInt64Array): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: number): number
    function fill(self, arg0: number): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: number): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedInt64Array
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: number, arg1: (boolean)?): number
    function duplicate(self): PackedInt64Array
    function find(self, arg0: number, arg1: (number)?): number
    function rfind(self, arg0: number, arg1: (number)?): number
    function count(self, arg0: number): number
    function erase(self, arg0: number): boolean
end

declare extern type PackedFloat32Array with
    function get(self, arg0: number): number
    function set(self, arg0: number, arg1: number): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: number): boolean
    function append(self, arg0: number): boolean
    function appendArray(self, arg0: PackedFloat32Array): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: number): number
    function fill(self, arg0: number): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: number): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedFloat32Array
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: number, arg1: (boolean)?): number
    function duplicate(self): PackedFloat32Array
    function find(self, arg0: number, arg1: (number)?): number
    function rfind(self, arg0: number, arg1: (number)?): number
    function count(self, arg0: number): number
    function erase(self, arg0: number): boolean
end

declare extern type PackedFloat64Array with
    function get(self, arg0: number): number
    function set(self, arg0: number, arg1: number): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: number): boolean
    function append(self, arg0: number): boolean
    function appendArray(self, arg0: PackedFloat64Array): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: number): number
    function fill(self, arg0: number): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: number): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedFloat64Array
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: number, arg1: (boolean)?): number
    function duplicate(self): PackedFloat64Array
    function find(self, arg0: number, arg1: (number)?): number
    function rfind(self, arg0: number, arg1: (number)?): number
    function count(self, arg0: number): number
    function erase(self, arg0: number): boolean
end

declare extern type PackedStringArray with
    function get(self, arg0: number): string
    function set(self, arg0: number, arg1: string): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: string): boolean
    function append(self, arg0: string): boolean
    function appendArray(self, arg0: PackedStringArray): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: string): number
    function fill(self, arg0: string): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: string): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedStringArray
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: string, arg1: (boolean)?): number
    function duplicate(self): PackedStringArray
    function find(self, arg0: string, arg1: (number)?): number
    function rfind(self, arg0: string, arg1: (number)?): number
    function count(self, arg0: string): number
    function erase(self, arg0: string): boolean
end

declare extern type PackedVector2Array with
    function get(self, arg0: number): Vector2
    function set(self, arg0: number, arg1: Vector2): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: Vector2): boolean
    function append(self, arg0: Vector2): boolean
    function appendArray(self, arg0: PackedVector2Array): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: Vector2): number
    function fill(self, arg0: Vector2): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: Vector2): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedVector2Array
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: Vector2, arg1: (boolean)?): number
    function duplicate(self): PackedVector2Array
    function find(self, arg0: Vector2, arg1: (number)?): number
    function rfind(self, arg0: Vector2, arg1: (number)?): number
    function count(self, arg0: Vector2): number
    function erase(self, arg0: Vector2): boolean
end

declare extern type PackedVector3Array with
    function get(self, arg0: number): Vector3
    function set(self, arg0: number, arg1: Vector3): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: Vector3): boolean
    function append(self, arg0: Vector3): boolean
    function appendArray(self, arg0: PackedVector3Array): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: Vector3): number
    function fill(self, arg0: Vector3): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: Vector3): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedVector3Array
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: Vector3, arg1: (boolean)?): number
    function duplicate(self): PackedVector3Array
    function find(self, arg0: Vector3, arg1: (number)?): number
    function rfind(self, arg0: Vector3, arg1: (number)?): number
    function count(self, arg0: Vector3): number
    function erase(self, arg0: Vector3): boolean
end

declare extern type PackedColorArray with
    function get(self, arg0: number): Color
    function set(self, arg0: number, arg1: Color): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: Color): boolean
    function append(self, arg0: Color): boolean
    function appendArray(self, arg0: PackedColorArray): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: Color): number
    function fill(self, arg0: Color): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: Color): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedColorArray
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: Color, arg1: (boolean)?): number
    function duplicate(self): PackedColorArray
    function find(self, arg0: Color, arg1: (number)?): number
    function rfind(self, arg0: Color, arg1: (number)?): number
    function count(self, arg0: Color): number
    function erase(self, arg0: Color): boolean
end

declare extern type PackedVector4Array with
    function get(self, arg0: number): Vector4
    function set(self, arg0: number, arg1: Vector4): ()
    function size(self): number
    function isEmpty(self): boolean
    function pushBack(self, arg0: Vector4): boolean
    function append(self, arg0: Vector4): boolean
    function appendArray(self, arg0: PackedVector4Array): ()
    function removeAt(self, arg0: number): ()
    function insert(self, arg0: number, arg1: Vector4): number
    function fill(self, arg0: Vector4): ()
    function resize(self, arg0: number): number
    function clear(self): ()
    function has(self, arg0: Vector4): boolean
    function reverse(self): ()
    function slice(self, arg0: number, arg1: (number)?): PackedVector4Array
    function toByteArray(self): PackedByteArray
    function sort(self): ()
    function bsearch(self, arg0: Vector4, arg1: (boolean)?): number
    function duplicate(self): PackedVector4Array
    function find(self, arg0: Vector4, arg1: (number)?): number
    function rfind(self, arg0: Vector4, arg1: (number)?): number
    function count(self, arg0: Vector4): number
    function erase(self, arg0: Vector4): boolean
end

declare extern type Object with
    function getClass(self): string
    function isClass(self, arg0: string): boolean
    function set(self, arg0: string, arg1: any): ()
    function get(self, arg0: string): any
    function setIndexed(self, arg0: string, arg1: any): ()
    function getIndexed(self, arg0: string): any
    function getPropertyList(self): Array
    function getMethodList(self): Array
    function propertyCanRevert(self, arg0: string): boolean
    function propertyGetRevert(self, arg0: string): any
    function notification(self, arg0: number, arg1: (boolean)?): ()
    function toString(self): string
    function getInstanceId(self): number
    function setScript(self, arg0: any): ()
    function getScript(self): any
    function setMeta(self, arg0: string, arg1: any): ()
    function removeMeta(self, arg0: string): ()
    function getMeta(self, arg0: string, arg1: (any)?): any
    function hasMeta(self, arg0: string): boolean
    function getMetaList(self): Array
    function addUserSignal(self, arg0: string, arg1: (Array)?): ()
    function hasUserSignal(self, arg0: string): boolean
    function removeUserSignal(self, arg0: string): ()
    function emitSignal(self, arg0: string, ...: any): number
    function call(self, arg0: string, ...: any): any
    function callDeferred(self, arg0: string, ...: any): any
    function setDeferred(self, arg0: string, arg1: any): ()
    function callv(self, arg0: string, arg1: Array): any
    function hasMethod(self, arg0: string): boolean
    function getMethodArgumentCount(self, arg0: string): number
    function hasSignal(self, arg0: string): boolean
    function getSignalList(self): Array
    function getSignalConnectionList(self, arg0: string): Array
    function getIncomingConnections(self): Array
    function connect(self, arg0: string, arg1: Callable, arg2: (number)?): number
    function disconnect(self, arg0: string, arg1: Callable): ()
    function isConnected(self, arg0: string, arg1: Callable): boolean
    function hasConnections(self, arg0: string): boolean
    function setBlockSignals(self, arg0: boolean): ()
    function isBlockingSignals(self): boolean
    function notifyPropertyListChanged(self): ()
    function setMessageTranslation(self, arg0: boolean): ()
    function canTranslateMessages(self): boolean
    function tr(self, arg0: string, arg1: (string)?): string
    function trN(self, arg0: string, arg1: string, arg2: number, arg3: (string)?): string
    function getTranslationDomain(self): string
    function setTranslationDomain(self, arg0: string): ()
    function isQueuedForDeletion(self): boolean
    function cancelFree(self): ()
end

declare extern type Node extends Object with
    Name: string
    UniqueNameInOwner: boolean
    SceneFilePath: string
    Owner: Node
    Multiplayer: any
    ProcessMode: number
    ProcessPriority: number
    ProcessPhysicsPriority: number
    ProcessThreadGroup: number
    ProcessThreadGroupOrder: number
    ProcessThreadMessages: number
    PhysicsInterpolationMode: number
    AutoTranslateMode: number
    EditorDescription: string
    function _process(self, arg0: number): ()
    function _physicsProcess(self, arg0: number): ()
    function _enterTree(self): ()
    function _exitTree(self): ()
    function _ready(self): ()
    function _getConfigurationWarnings(self): PackedStringArray
    function _getAccessibilityConfigurationWarnings(self): PackedStringArray
    function _input(self, arg0: InputEvent): ()
    function _shortcutInput(self, arg0: InputEvent): ()
    function _unhandledInput(self, arg0: InputEvent): ()
    function _unhandledKeyInput(self, arg0: InputEvent): ()
    function _getFocusedAccessibilityElement(self): RID
    function printOrphanNodes(self): ()
    function getOrphanNodeIds(self): Array
    function addSibling(self, arg0: Node, arg1: (boolean)?): ()
    function setName(self, arg0: string): ()
    function getName(self): string
    function addChild(self, arg0: Node, arg1: (boolean)?, arg2: (number)?): ()
    function removeChild(self, arg0: Node): ()
    function reparent(self, arg0: Node, arg1: (boolean)?): ()
    function getChildCount(self, arg0: (boolean)?): number
    function getChildren(self, arg0: (boolean)?): Array
    function getChild(self, arg0: number, arg1: (boolean)?): Node
    function hasNode(self, arg0: string): boolean
    function getNode(self, arg0: string): Node
    function getNodeOrNull(self, arg0: string): Node
    function getParent(self): Node
    function findChild(self, arg0: string, arg1: (boolean)?, arg2: (boolean)?): Node
    function findChildren(self, arg0: string, arg1: (string)?, arg2: (boolean)?, arg3: (boolean)?): Array
    function findParent(self, arg0: string): Node
    function hasNodeAndResource(self, arg0: string): boolean
    function getNodeAndResource(self, arg0: string): Array
    function isInsideTree(self): boolean
    function isPartOfEditedScene(self): boolean
    function isAncestorOf(self, arg0: Node): boolean
    function isGreaterThan(self, arg0: Node): boolean
    function getPath(self): string
    function getPathTo(self, arg0: Node, arg1: (boolean)?): string
    function addToGroup(self, arg0: string, arg1: (boolean)?): ()
    function removeFromGroup(self, arg0: string): ()
    function isInGroup(self, arg0: string): boolean
    function moveChild(self, arg0: Node, arg1: number): ()
    function getGroups(self): Array
    function setOwner(self, arg0: Node): ()
    function getOwner(self): Node
    function getIndex(self, arg0: (boolean)?): number
    function printTree(self): ()
    function printTreePretty(self): ()
    function getTreeString(self): string
    function getTreeStringPretty(self): string
    function setSceneFilePath(self, arg0: string): ()
    function getSceneFilePath(self): string
    function propagateNotification(self, arg0: number): ()
    function propagateCall(self, arg0: string, arg1: (Array)?, arg2: (boolean)?): ()
    function setPhysicsProcess(self, arg0: boolean): ()
    function getPhysicsProcessDeltaTime(self): number
    function isPhysicsProcessing(self): boolean
    function getProcessDeltaTime(self): number
    function setProcess(self, arg0: boolean): ()
    function setProcessPriority(self, arg0: number): ()
    function getProcessPriority(self): number
    function setPhysicsProcessPriority(self, arg0: number): ()
    function getPhysicsProcessPriority(self): number
    function isProcessing(self): boolean
    function setProcessInput(self, arg0: boolean): ()
    function isProcessingInput(self): boolean
    function setProcessShortcutInput(self, arg0: boolean): ()
    function isProcessingShortcutInput(self): boolean
    function setProcessUnhandledInput(self, arg0: boolean): ()
    function isProcessingUnhandledInput(self): boolean
    function setProcessUnhandledKeyInput(self, arg0: boolean): ()
    function isProcessingUnhandledKeyInput(self): boolean
    function setProcessMode(self, arg0: number): ()
    function getProcessMode(self): number
    function canProcess(self): boolean
    function setProcessThreadGroup(self, arg0: number): ()
    function getProcessThreadGroup(self): number
    function setProcessThreadMessages(self, arg0: number): ()
    function getProcessThreadMessages(self): number
    function setProcessThreadGroupOrder(self, arg0: number): ()
    function getProcessThreadGroupOrder(self): number
    function queueAccessibilityUpdate(self): ()
    function getAccessibilityElement(self): RID
    function setDisplayFolded(self, arg0: boolean): ()
    function isDisplayedFolded(self): boolean
    function setProcessInternal(self, arg0: boolean): ()
    function isProcessingInternal(self): boolean
    function setPhysicsProcessInternal(self, arg0: boolean): ()
    function isPhysicsProcessingInternal(self): boolean
    function setPhysicsInterpolationMode(self, arg0: number): ()
    function getPhysicsInterpolationMode(self): number
    function isPhysicsInterpolated(self): boolean
    function isPhysicsInterpolatedAndEnabled(self): boolean
    function resetPhysicsInterpolation(self): ()
    function setAutoTranslateMode(self, arg0: number): ()
    function getAutoTranslateMode(self): number
    function canAutoTranslate(self): boolean
    function setTranslationDomainInherited(self): ()
    function getWindow(self): any
    function getLastExclusiveWindow(self): any
    function getTree(self): SceneTree
    function createTween(self): any
    function duplicate(self, arg0: (number)?): Node
    function replaceBy(self, arg0: Node, arg1: (boolean)?): ()
    function setSceneInstanceLoadPlaceholder(self, arg0: boolean): ()
    function getSceneInstanceLoadPlaceholder(self): boolean
    function setEditableInstance(self, arg0: Node, arg1: boolean): ()
    function isEditableInstance(self, arg0: Node): boolean
    function getViewport(self): any
    function queueFree(self): ()
    function requestReady(self): ()
    function isNodeReady(self): boolean
    function setMultiplayerAuthority(self, arg0: number, arg1: (boolean)?): ()
    function getMultiplayerAuthority(self): number
    function isMultiplayerAuthority(self): boolean
    function getMultiplayer(self): any
    function rpcConfig(self, arg0: string, arg1: any): ()
    function getNodeRpcConfig(self): any
    function setEditorDescription(self, arg0: string): ()
    function getEditorDescription(self): string
    function setUniqueNameInOwner(self, arg0: boolean): ()
    function isUniqueNameInOwner(self): boolean
    function atr(self, arg0: string, arg1: (string)?): string
    function atrN(self, arg0: string, arg1: string, arg2: number, arg3: (string)?): string
    function rpc(self, arg0: string, ...: any): number
    function rpcId(self, arg0: number, arg1: string, ...: any): number
    function updateConfigurationWarnings(self): ()
    function callDeferredThreadGroup(self, arg0: string, ...: any): any
    function setDeferredThreadGroup(self, arg0: string, arg1: any): ()
    function notifyDeferredThreadGroup(self, arg0: number): ()
    function callThreadSafe(self, arg0: string, ...: any): any
    function setThreadSafe(self, arg0: string, arg1: any): ()
    function notifyThreadSafe(self, arg0: number): ()
end

declare extern type CanvasItem extends Node with
    Visible: boolean
    Modulate: Color
    SelfModulate: Color
    ShowBehindParent: boolean
    TopLevel: boolean
    ClipChildren: number
    OversamplingWithScale: number
    LightMask: number
    VisibilityLayer: number
    ZIndex: number
    ZAsRelative: boolean
    YSortEnabled: boolean
    TextureFilter: number
    TextureRepeat: number
    Material: any
    UseParentMaterial: boolean
    function _draw(self): ()
    function getCanvasItem(self): RID
    function setVisible(self, arg0: boolean): ()
    function isVisible(self): boolean
    function isVisibleInTree(self): boolean
    function show(self): ()
    function hide(self): ()
    function queueRedraw(self): ()
    function moveToFront(self): ()
    function setAsTopLevel(self, arg0: boolean): ()
    function isSetAsTopLevel(self): boolean
    function setLightMask(self, arg0: number): ()
    function getLightMask(self): number
    function setModulate(self, arg0: Color): ()
    function getModulate(self): Color
    function setSelfModulate(self, arg0: Color): ()
    function getSelfModulate(self): Color
    function setZIndex(self, arg0: number): ()
    function getZIndex(self): number
    function setZAsRelative(self, arg0: boolean): ()
    function isZRelative(self): boolean
    function setYSortEnabled(self, arg0: boolean): ()
    function isYSortEnabled(self): boolean
    function setDrawBehindParent(self, arg0: boolean): ()
    function isDrawBehindParentEnabled(self): boolean
    function drawLine(self, arg0: Vector2, arg1: Vector2, arg2: Color, arg3: (number)?, arg4: (boolean)?): ()
    function drawDashedLine(self, arg0: Vector2, arg1: Vector2, arg2: Color, arg3: (number)?, arg4: (number)?, arg5: (boolean)?, arg6: (boolean)?): ()
    function drawPolyline(self, arg0: PackedVector2Array, arg1: Color, arg2: (number)?, arg3: (boolean)?): ()
    function drawPolylineColors(self, arg0: PackedVector2Array, arg1: PackedColorArray, arg2: (number)?, arg3: (boolean)?): ()
    function drawEllipseArc(self, arg0: Vector2, arg1: number, arg2: number, arg3: number, arg4: number, arg5: number, arg6: Color, arg7: (number)?, arg8: (boolean)?): ()
    function drawArc(self, arg0: Vector2, arg1: number, arg2: number, arg3: number, arg4: number, arg5: Color, arg6: (number)?, arg7: (boolean)?): ()
    function drawMultiline(self, arg0: PackedVector2Array, arg1: Color, arg2: (number)?, arg3: (boolean)?): ()
    function drawMultilineColors(self, arg0: PackedVector2Array, arg1: PackedColorArray, arg2: (number)?, arg3: (boolean)?): ()
    function drawRect(self, arg0: Rect2, arg1: Color, arg2: (boolean)?, arg3: (number)?, arg4: (boolean)?): ()
    function drawCircle(self, arg0: Vector2, arg1: number, arg2: Color, arg3: (boolean)?, arg4: (number)?, arg5: (boolean)?): ()
    function drawEllipse(self, arg0: Vector2, arg1: number, arg2: number, arg3: Color, arg4: (boolean)?, arg5: (number)?, arg6: (boolean)?): ()
    function drawTexture(self, arg0: Texture2D, arg1: Vector2, arg2: (Color)?): ()
    function drawTextureRect(self, arg0: Texture2D, arg1: Rect2, arg2: boolean, arg3: (Color)?, arg4: (boolean)?): ()
    function drawTextureRectRegion(self, arg0: Texture2D, arg1: Rect2, arg2: Rect2, arg3: (Color)?, arg4: (boolean)?, arg5: (boolean)?): ()
    function drawMsdfTextureRectRegion(self, arg0: Texture2D, arg1: Rect2, arg2: Rect2, arg3: (Color)?, arg4: (number)?, arg5: (number)?, arg6: (number)?): ()
    function drawLcdTextureRectRegion(self, arg0: Texture2D, arg1: Rect2, arg2: Rect2, arg3: (Color)?): ()
    function drawStyleBox(self, arg0: any, arg1: Rect2): ()
    function drawPrimitive(self, arg0: PackedVector2Array, arg1: PackedColorArray, arg2: PackedVector2Array, arg3: (Texture2D)?): ()
    function drawPolygon(self, arg0: PackedVector2Array, arg1: PackedColorArray, arg2: (PackedVector2Array)?, arg3: (Texture2D)?): ()
    function drawColoredPolygon(self, arg0: PackedVector2Array, arg1: Color, arg2: (PackedVector2Array)?, arg3: (Texture2D)?): ()
    function drawString(self, arg0: any, arg1: Vector2, arg2: string, arg3: (number)?, arg4: (number)?, arg5: (number)?, arg6: (Color)?, arg7: (number)?, arg8: (number)?, arg9: (number)?, arg10: (number)?): ()
    function drawMultilineString(self, arg0: any, arg1: Vector2, arg2: string, arg3: (number)?, arg4: (number)?, arg5: (number)?, arg6: (number)?, arg7: (Color)?, arg8: (number)?, arg9: (number)?, arg10: (number)?, arg11: (number)?, arg12: (number)?): ()
    function drawStringOutline(self, arg0: any, arg1: Vector2, arg2: string, arg3: (number)?, arg4: (number)?, arg5: (number)?, arg6: (number)?, arg7: (Color)?, arg8: (number)?, arg9: (number)?, arg10: (number)?, arg11: (number)?): ()
    function drawMultilineStringOutline(self, arg0: any, arg1: Vector2, arg2: string, arg3: (number)?, arg4: (number)?, arg5: (number)?, arg6: (number)?, arg7: (number)?, arg8: (Color)?, arg9: (number)?, arg10: (number)?, arg11: (number)?, arg12: (number)?, arg13: (number)?): ()
    function drawChar(self, arg0: any, arg1: Vector2, arg2: string, arg3: (number)?, arg4: (Color)?, arg5: (number)?): ()
    function drawCharOutline(self, arg0: any, arg1: Vector2, arg2: string, arg3: (number)?, arg4: (number)?, arg5: (Color)?, arg6: (number)?): ()
    function drawMesh(self, arg0: any, arg1: Texture2D, arg2: (Transform2D)?, arg3: (Color)?): ()
    function drawMultimesh(self, arg0: any, arg1: Texture2D): ()
    function drawSetTransform(self, arg0: Vector2, arg1: (number)?, arg2: (Vector2)?): ()
    function drawSetTransformMatrix(self, arg0: Transform2D): ()
    function drawAnimationSlice(self, arg0: number, arg1: number, arg2: number, arg3: (number)?): ()
    function drawEndAnimation(self): ()
    function getTransform(self): Transform2D
    function getGlobalTransform(self): Transform2D
    function getGlobalTransformWithCanvas(self): Transform2D
    function getViewportTransform(self): Transform2D
    function getViewportRect(self): Rect2
    function getCanvasTransform(self): Transform2D
    function getScreenTransform(self): Transform2D
    function getLocalMousePosition(self): Vector2
    function getGlobalMousePosition(self): Vector2
    function getCanvas(self): RID
    function getCanvasLayerNode(self): any
    function getWorld2d(self): any
    function setMaterial(self, arg0: Material): ()
    function getMaterial(self): Material
    function setInstanceShaderParameter(self, arg0: string, arg1: any): ()
    function getInstanceShaderParameter(self, arg0: string): any
    function setUseParentMaterial(self, arg0: boolean): ()
    function getUseParentMaterial(self): boolean
    function setNotifyLocalTransform(self, arg0: boolean): ()
    function isLocalTransformNotificationEnabled(self): boolean
    function setNotifyTransform(self, arg0: boolean): ()
    function isTransformNotificationEnabled(self): boolean
    function forceUpdateTransform(self): ()
    function makeCanvasPositionLocal(self, arg0: Vector2): Vector2
    function makeInputLocal(self, arg0: InputEvent): InputEvent
    function setVisibilityLayer(self, arg0: number): ()
    function getVisibilityLayer(self): number
    function setVisibilityLayerBit(self, arg0: number, arg1: boolean): ()
    function getVisibilityLayerBit(self, arg0: number): boolean
    function setTextureFilter(self, arg0: number): ()
    function getTextureFilter(self): number
    function setTextureRepeat(self, arg0: number): ()
    function getTextureRepeat(self): number
    function setClipChildrenMode(self, arg0: number): ()
    function getClipChildrenMode(self): number
    function setOversamplingWithScale(self, arg0: number): ()
    function getOversamplingWithScale(self): number
end

declare extern type Node2D extends CanvasItem with
    Position: Vector2
    Rotation: number
    RotationDegrees: number
    Scale: Vector2
    Skew: number
    Transform: Transform2D
    GlobalPosition: Vector2
    GlobalRotation: number
    GlobalRotationDegrees: number
    GlobalScale: Vector2
    GlobalSkew: number
    GlobalTransform: Transform2D
    function setPosition(self, arg0: Vector2): ()
    function setRotation(self, arg0: number): ()
    function setRotationDegrees(self, arg0: number): ()
    function setSkew(self, arg0: number): ()
    function setScale(self, arg0: Vector2): ()
    function getPosition(self): Vector2
    function getRotation(self): number
    function getRotationDegrees(self): number
    function getSkew(self): number
    function getScale(self): Vector2
    function rotate(self, arg0: number): ()
    function moveLocalX(self, arg0: number, arg1: (boolean)?): ()
    function moveLocalY(self, arg0: number, arg1: (boolean)?): ()
    function translate(self, arg0: Vector2): ()
    function globalTranslate(self, arg0: Vector2): ()
    function applyScale(self, arg0: Vector2): ()
    function setGlobalPosition(self, arg0: Vector2): ()
    function getGlobalPosition(self): Vector2
    function setGlobalRotation(self, arg0: number): ()
    function setGlobalRotationDegrees(self, arg0: number): ()
    function getGlobalRotation(self): number
    function getGlobalRotationDegrees(self): number
    function setGlobalSkew(self, arg0: number): ()
    function getGlobalSkew(self): number
    function setGlobalScale(self, arg0: Vector2): ()
    function getGlobalScale(self): Vector2
    function setTransform(self, arg0: Transform2D): ()
    function setGlobalTransform(self, arg0: Transform2D): ()
    function lookAt(self, arg0: Vector2): ()
    function getAngleTo(self, arg0: Vector2): number
    function toLocal(self, arg0: Vector2): Vector2
    function toGlobal(self, arg0: Vector2): Vector2
    function getRelativeTransformToParent(self, arg0: Node): Transform2D
end

declare extern type CollisionObject2D extends Node2D with
    DisableMode: number
    CollisionLayer: number
    CollisionMask: number
    CollisionPriority: number
    InputPickable: boolean
    function _inputEvent(self, arg0: any, arg1: InputEvent, arg2: number): ()
    function _mouseEnter(self): ()
    function _mouseExit(self): ()
    function _mouseShapeEnter(self, arg0: number): ()
    function _mouseShapeExit(self, arg0: number): ()
    function getRid(self): RID
    function setCollisionLayer(self, arg0: number): ()
    function getCollisionLayer(self): number
    function setCollisionMask(self, arg0: number): ()
    function getCollisionMask(self): number
    function setCollisionLayerValue(self, arg0: number, arg1: boolean): ()
    function getCollisionLayerValue(self, arg0: number): boolean
    function setCollisionMaskValue(self, arg0: number, arg1: boolean): ()
    function getCollisionMaskValue(self, arg0: number): boolean
    function setCollisionPriority(self, arg0: number): ()
    function getCollisionPriority(self): number
    function setDisableMode(self, arg0: number): ()
    function getDisableMode(self): number
    function setPickable(self, arg0: boolean): ()
    function isPickable(self): boolean
    function createShapeOwner(self, arg0: Object): number
    function removeShapeOwner(self, arg0: number): ()
    function getShapeOwners(self): PackedInt32Array
    function shapeOwnerSetTransform(self, arg0: number, arg1: Transform2D): ()
    function shapeOwnerGetTransform(self, arg0: number): Transform2D
    function shapeOwnerGetOwner(self, arg0: number): Object
    function shapeOwnerSetDisabled(self, arg0: number, arg1: boolean): ()
    function isShapeOwnerDisabled(self, arg0: number): boolean
    function shapeOwnerSetOneWayCollision(self, arg0: number, arg1: boolean): ()
    function isShapeOwnerOneWayCollisionEnabled(self, arg0: number): boolean
    function shapeOwnerSetOneWayCollisionMargin(self, arg0: number, arg1: number): ()
    function getShapeOwnerOneWayCollisionMargin(self, arg0: number): number
    function getShapeOwnerOneWayCollisionDirection(self, arg0: number): Vector2
    function shapeOwnerSetOneWayCollisionDirection(self, arg0: number, arg1: Vector2): ()
    function shapeOwnerAddShape(self, arg0: number, arg1: any): ()
    function shapeOwnerGetShapeCount(self, arg0: number): number
    function shapeOwnerGetShape(self, arg0: number, arg1: number): any
    function shapeOwnerGetShapeIndex(self, arg0: number, arg1: number): number
    function shapeOwnerRemoveShape(self, arg0: number, arg1: number): ()
    function shapeOwnerClearShapes(self, arg0: number): ()
    function shapeFindOwner(self, arg0: number): number
end

declare extern type PhysicsBody2D extends CollisionObject2D with
    function moveAndCollide(self, arg0: Vector2, arg1: (boolean)?, arg2: (number)?, arg3: (boolean)?): any
    function testMove(self, arg0: Transform2D, arg1: Vector2, arg2: (any)?, arg3: (number)?, arg4: (boolean)?): boolean
    function getGravity(self): Vector2
    function getCollisionExceptions(self): Array
    function addCollisionExceptionWith(self, arg0: Node): ()
    function removeCollisionExceptionWith(self, arg0: Node): ()
end

declare extern type CharacterBody2D extends PhysicsBody2D with
    MotionMode: number
    UpDirection: Vector2
    Velocity: Vector2
    SlideOnCeiling: boolean
    MaxSlides: number
    WallMinSlideAngle: number
    FloorStopOnSlope: boolean
    FloorConstantSpeed: boolean
    FloorBlockOnWall: boolean
    FloorMaxAngle: number
    FloorSnapLength: number
    PlatformOnLeave: number
    PlatformFloorLayers: number
    PlatformWallLayers: number
    SafeMargin: number
    function moveAndSlide(self): boolean
    function applyFloorSnap(self): ()
    function setVelocity(self, arg0: Vector2): ()
    function getVelocity(self): Vector2
    function setSafeMargin(self, arg0: number): ()
    function getSafeMargin(self): number
    function isFloorStopOnSlopeEnabled(self): boolean
    function setFloorStopOnSlopeEnabled(self, arg0: boolean): ()
    function setFloorConstantSpeedEnabled(self, arg0: boolean): ()
    function isFloorConstantSpeedEnabled(self): boolean
    function setFloorBlockOnWallEnabled(self, arg0: boolean): ()
    function isFloorBlockOnWallEnabled(self): boolean
    function setSlideOnCeilingEnabled(self, arg0: boolean): ()
    function isSlideOnCeilingEnabled(self): boolean
    function setPlatformFloorLayers(self, arg0: number): ()
    function getPlatformFloorLayers(self): number
    function setPlatformWallLayers(self, arg0: number): ()
    function getPlatformWallLayers(self): number
    function getMaxSlides(self): number
    function setMaxSlides(self, arg0: number): ()
    function getFloorMaxAngle(self): number
    function setFloorMaxAngle(self, arg0: number): ()
    function getFloorSnapLength(self): number
    function setFloorSnapLength(self, arg0: number): ()
    function getWallMinSlideAngle(self): number
    function setWallMinSlideAngle(self, arg0: number): ()
    function getUpDirection(self): Vector2
    function setUpDirection(self, arg0: Vector2): ()
    function setMotionMode(self, arg0: number): ()
    function getMotionMode(self): number
    function setPlatformOnLeave(self, arg0: number): ()
    function getPlatformOnLeave(self): number
    function isOnFloor(self): boolean
    function isOnFloorOnly(self): boolean
    function isOnCeiling(self): boolean
    function isOnCeilingOnly(self): boolean
    function isOnWall(self): boolean
    function isOnWallOnly(self): boolean
    function getFloorNormal(self): Vector2
    function getWallNormal(self): Vector2
    function getLastMotion(self): Vector2
    function getPositionDelta(self): Vector2
    function getRealVelocity(self): Vector2
    function getFloorAngle(self, arg0: (Vector2)?): number
    function getPlatformVelocity(self): Vector2
    function getSlideCollisionCount(self): number
    function getSlideCollision(self, arg0: number): any
    function getLastSlideCollision(self): any
end

declare extern type Engine extends Object with
    PrintErrorMessages: boolean
    PrintToStdout: boolean
    PhysicsTicksPerSecond: number
    MaxPhysicsStepsPerFrame: number
    MaxFps: number
    TimeScale: number
    PhysicsJitterFix: number
    function setPhysicsTicksPerSecond(self, arg0: number): ()
    function getPhysicsTicksPerSecond(self): number
    function setMaxPhysicsStepsPerFrame(self, arg0: number): ()
    function getMaxPhysicsStepsPerFrame(self): number
    function setPhysicsJitterFix(self, arg0: number): ()
    function getPhysicsJitterFix(self): number
    function getPhysicsInterpolationFraction(self): number
    function setMaxFps(self, arg0: number): ()
    function getMaxFps(self): number
    function setTimeScale(self, arg0: number): ()
    function getTimeScale(self): number
    function getFramesDrawn(self): number
    function getFramesPerSecond(self): number
    function getPhysicsFrames(self): number
    function getProcessFrames(self): number
    function getMainLoop(self): MainLoop
    function getVersionInfo(self): Dictionary
    function getAuthorInfo(self): Dictionary
    function getCopyrightInfo(self): Array
    function getDonorInfo(self): Dictionary
    function getLicenseInfo(self): Dictionary
    function getLicenseText(self): string
    function getArchitectureName(self): string
    function isInPhysicsFrame(self): boolean
    function hasSingleton(self, arg0: string): boolean
    function getSingleton(self, arg0: string): Object
    function registerSingleton(self, arg0: string, arg1: Object): ()
    function unregisterSingleton(self, arg0: string): ()
    function getSingletonList(self): PackedStringArray
    function registerScriptLanguage(self, arg0: any): number
    function unregisterScriptLanguage(self, arg0: any): number
    function getScriptLanguageCount(self): number
    function getScriptLanguage(self, arg0: number): any
    function captureScriptBacktraces(self, arg0: (boolean)?): Array
    function isEditorHint(self): boolean
    function isEmbeddedInEditor(self): boolean
    function getWriteMoviePath(self): string
    function setPrintToStdout(self, arg0: boolean): ()
    function isPrintingToStdout(self): boolean
    function setPrintErrorMessages(self, arg0: boolean): ()
    function isPrintingErrorMessages(self): boolean
end

declare extern type Input extends Object with
    MouseMode: number
    UseAccumulatedInput: boolean
    EmulateMouseFromTouch: boolean
    EmulateTouchFromMouse: boolean
    IgnoreJoypadOnUnfocusedApplication: boolean
    function isAnythingPressed(self): boolean
    function isKeyPressed(self, arg0: number): boolean
    function isPhysicalKeyPressed(self, arg0: number): boolean
    function isKeyLabelPressed(self, arg0: number): boolean
    function isMouseButtonPressed(self, arg0: number): boolean
    function isJoyButtonPressed(self, arg0: number, arg1: number): boolean
    function isActionPressed(self, arg0: string, arg1: (boolean)?): boolean
    function isActionJustPressed(self, arg0: string, arg1: (boolean)?): boolean
    function isActionJustReleased(self, arg0: string, arg1: (boolean)?): boolean
    function isActionJustPressedByEvent(self, arg0: string, arg1: InputEvent, arg2: (boolean)?): boolean
    function isActionJustReleasedByEvent(self, arg0: string, arg1: InputEvent, arg2: (boolean)?): boolean
    function getActionStrength(self, arg0: string, arg1: (boolean)?): number
    function getActionRawStrength(self, arg0: string, arg1: (boolean)?): number
    function getAxis(self, arg0: string, arg1: string): number
    function getVector(self, arg0: string, arg1: string, arg2: string, arg3: string, arg4: (number)?): Vector2
    function addJoyMapping(self, arg0: string, arg1: (boolean)?): ()
    function removeJoyMapping(self, arg0: string): ()
    function isJoyKnown(self, arg0: number): boolean
    function getJoyAxis(self, arg0: number, arg1: number): number
    function getJoyName(self, arg0: number): string
    function getJoyGuid(self, arg0: number): string
    function getJoyInfo(self, arg0: number): Dictionary
    function shouldIgnoreDevice(self, arg0: number, arg1: number): boolean
    function getConnectedJoypads(self): Array
    function getJoyVibrationStrength(self, arg0: number): Vector2
    function getJoyVibrationDuration(self, arg0: number): number
    function getJoyVibrationRemainingDuration(self, arg0: number): number
    function isJoyVibrating(self, arg0: number): boolean
    function hasJoyVibration(self, arg0: number): boolean
    function startJoyVibration(self, arg0: number, arg1: number, arg2: number, arg3: (number)?): ()
    function stopJoyVibration(self, arg0: number): ()
    function vibrateHandheld(self, arg0: (number)?, arg1: (number)?): ()
    function setIgnoreJoypadOnUnfocusedApplication(self, arg0: boolean): ()
    function isIgnoringJoypadOnUnfocusedApplication(self): boolean
    function getGravity(self): Vector3
    function getAccelerometer(self): Vector3
    function getMagnetometer(self): Vector3
    function getGyroscope(self): Vector3
    function getJoyAccelerometer(self, arg0: number): Vector3
    function getJoyGravity(self, arg0: number): Vector3
    function getJoyGyroscope(self, arg0: number): Vector3
    function getJoyMotionSensorsRate(self, arg0: number): number
    function isJoyMotionSensorsEnabled(self, arg0: number): boolean
    function setJoyMotionSensorsEnabled(self, arg0: number, arg1: boolean): ()
    function hasJoyMotionSensors(self, arg0: number): boolean
    function startJoyMotionSensorsCalibration(self, arg0: number): ()
    function stopJoyMotionSensorsCalibration(self, arg0: number): ()
    function clearJoyMotionSensorsCalibration(self, arg0: number): ()
    function getJoyMotionSensorsCalibration(self, arg0: number): Dictionary
    function setJoyMotionSensorsCalibration(self, arg0: number, arg1: Dictionary): ()
    function isJoyMotionSensorsCalibrated(self, arg0: number): boolean
    function isJoyMotionSensorsCalibrating(self, arg0: number): boolean
    function setGravity(self, arg0: Vector3): ()
    function setAccelerometer(self, arg0: Vector3): ()
    function setMagnetometer(self, arg0: Vector3): ()
    function setGyroscope(self, arg0: Vector3): ()
    function setJoyLight(self, arg0: number, arg1: Color): ()
    function hasJoyLight(self, arg0: number): boolean
    function getLastMouseVelocity(self): Vector2
    function getLastMouseScreenVelocity(self): Vector2
    function getMouseButtonMask(self): number
    function setMouseMode(self, arg0: number): ()
    function getMouseMode(self): number
    function warpMouse(self, arg0: Vector2): ()
    function actionPress(self, arg0: string, arg1: (number)?): ()
    function actionRelease(self, arg0: string): ()
    function setDefaultCursorShape(self, arg0: (number)?): ()
    function getCurrentCursorShape(self): number
    function setCustomMouseCursor(self, arg0: Resource, arg1: (number)?, arg2: (Vector2)?): ()
    function parseInputEvent(self, arg0: InputEvent): ()
    function setUseAccumulatedInput(self, arg0: boolean): ()
    function isUsingAccumulatedInput(self): boolean
    function flushBufferedEvents(self): ()
    function setEmulateMouseFromTouch(self, arg0: boolean): ()
    function isEmulatingMouseFromTouch(self): boolean
    function setEmulateTouchFromMouse(self, arg0: boolean): ()
    function isEmulatingTouchFromMouse(self): boolean
end

declare extern type RefCounted extends Object with
    function initRef(self): boolean
    function reference(self): boolean
    function unreference(self): boolean
    function getReferenceCount(self): number
end

declare extern type Resource extends RefCounted with
    ResourceLocalToScene: boolean
    ResourcePath: string
    ResourceName: string
    ResourceSceneUniqueId: string
    function _setupLocalToScene(self): ()
    function _getRid(self): RID
    function _resetState(self): ()
    function _setPathCache(self, arg0: string): ()
    function setPath(self, arg0: string): ()
    function takeOverPath(self, arg0: string): ()
    function getPath(self): string
    function setPathCache(self, arg0: string): ()
    function setName(self, arg0: string): ()
    function getName(self): string
    function getRid(self): RID
    function setLocalToScene(self, arg0: boolean): ()
    function isLocalToScene(self): boolean
    function getLocalScene(self): Node
    function setupLocalToScene(self): ()
    function resetState(self): ()
    function setIdForPath(self, arg0: string, arg1: string): ()
    function getIdForPath(self, arg0: string): string
    function isBuiltIn(self): boolean
    function generateSceneUniqueId(self): string
    function setSceneUniqueId(self, arg0: string): ()
    function getSceneUniqueId(self): string
    function emitChanged(self): ()
    function duplicate(self, arg0: (boolean)?): Resource
    function duplicateDeep(self, arg0: (number)?): Resource
    function copyFromResource(self, arg0: Resource): number
end

declare extern type InputEvent extends Resource with
    Device: number
    function setDevice(self, arg0: number): ()
    function getDevice(self): number
    function isAction(self, arg0: string, arg1: (boolean)?): boolean
    function isActionPressed(self, arg0: string, arg1: (boolean)?, arg2: (boolean)?): boolean
    function isActionReleased(self, arg0: string, arg1: (boolean)?): boolean
    function getActionStrength(self, arg0: string, arg1: (boolean)?): number
    function isCanceled(self): boolean
    function isPressed(self): boolean
    function isReleased(self): boolean
    function isEcho(self): boolean
    function asText(self): string
    function isMatch(self, arg0: InputEvent, arg1: (boolean)?): boolean
    function isActionType(self): boolean
    function accumulate(self, arg0: InputEvent): boolean
    function xformedBy(self, arg0: Transform2D, arg1: (Vector2)?): InputEvent
end

declare extern type InputEventFromWindow extends InputEvent with
    WindowId: number
    function setWindowId(self, arg0: number): ()
    function getWindowId(self): number
end

declare extern type InputEventWithModifiers extends InputEventFromWindow with
    CommandOrControlAutoremap: boolean
    AltPressed: boolean
    ShiftPressed: boolean
    CtrlPressed: boolean
    MetaPressed: boolean
    function setCommandOrControlAutoremap(self, arg0: boolean): ()
    function isCommandOrControlAutoremap(self): boolean
    function isCommandOrControlPressed(self): boolean
    function setAltPressed(self, arg0: boolean): ()
    function isAltPressed(self): boolean
    function setShiftPressed(self, arg0: boolean): ()
    function isShiftPressed(self): boolean
    function setCtrlPressed(self, arg0: boolean): ()
    function isCtrlPressed(self): boolean
    function setMetaPressed(self, arg0: boolean): ()
    function isMetaPressed(self): boolean
    function getModifiersMask(self): number
end

declare extern type InputEventKey extends InputEventWithModifiers with
    Pressed: boolean
    Keycode: number
    PhysicalKeycode: number
    KeyLabel: number
    Unicode: number
    Location: number
    Echo: boolean
    function setPressed(self, arg0: boolean): ()
    function setKeycode(self, arg0: number): ()
    function getKeycode(self): number
    function setPhysicalKeycode(self, arg0: number): ()
    function getPhysicalKeycode(self): number
    function setKeyLabel(self, arg0: number): ()
    function getKeyLabel(self): number
    function setUnicode(self, arg0: number): ()
    function getUnicode(self): number
    function setLocation(self, arg0: number): ()
    function getLocation(self): number
    function setEcho(self, arg0: boolean): ()
    function getKeycodeWithModifiers(self): number
    function getPhysicalKeycodeWithModifiers(self): number
    function getKeyLabelWithModifiers(self): number
    function asTextKeycode(self): string
    function asTextPhysicalKeycode(self): string
    function asTextKeyLabel(self): string
    function asTextLocation(self): string
end

declare extern type InputMap extends Object with
    function hasAction(self, arg0: string): boolean
    function getActions(self): Array
    function addAction(self, arg0: string, arg1: (number)?): ()
    function eraseAction(self, arg0: string): ()
    function getActionDescription(self, arg0: string): string
    function actionSetDeadzone(self, arg0: string, arg1: number): ()
    function actionGetDeadzone(self, arg0: string): number
    function actionAddEvent(self, arg0: string, arg1: InputEvent): ()
    function actionHasEvent(self, arg0: string, arg1: InputEvent): boolean
    function actionEraseEvent(self, arg0: string, arg1: InputEvent): ()
    function actionEraseEvents(self, arg0: string): ()
    function actionGetEvents(self, arg0: string): Array
    function eventIsAction(self, arg0: InputEvent, arg1: string, arg2: (boolean)?): boolean
    function loadFromProjectSettings(self): ()
end

declare extern type MainLoop extends Object with
    function _initialize(self): ()
    function _physicsProcess(self, arg0: number): boolean
    function _process(self, arg0: number): boolean
    function _finalize(self): ()
end

declare extern type Material extends Resource with
    RenderPriority: number
    NextPass: Material
    function _getShaderRid(self): RID
    function _getShaderMode(self): number
    function _canDoNextPass(self): boolean
    function _canUseRenderPriority(self): boolean
    function setNextPass(self, arg0: Material): ()
    function getNextPass(self): Material
    function setRenderPriority(self, arg0: number): ()
    function getRenderPriority(self): number
    function inspectNativeShaderCode(self): ()
    function createPlaceholder(self): Resource
end

declare extern type OS extends Object with
    LowProcessorUsageMode: boolean
    LowProcessorUsageModeSleepUsec: number
    DeltaSmoothing: boolean
    function getEntropy(self, arg0: number): PackedByteArray
    function getSystemCaCertificates(self): string
    function getConnectedMidiInputs(self): PackedStringArray
    function openMidiInputs(self): ()
    function closeMidiInputs(self): ()
    function alert(self, arg0: string, arg1: (string)?): ()
    function crash(self, arg0: string): ()
    function setLowProcessorUsageMode(self, arg0: boolean): ()
    function isInLowProcessorUsageMode(self): boolean
    function setLowProcessorUsageModeSleepUsec(self, arg0: number): ()
    function getLowProcessorUsageModeSleepUsec(self): number
    function setDeltaSmoothing(self, arg0: boolean): ()
    function isDeltaSmoothingEnabled(self): boolean
    function getProcessorCount(self): number
    function getProcessorName(self): string
    function getSystemFonts(self): PackedStringArray
    function getSystemFontPath(self, arg0: string, arg1: (number)?, arg2: (number)?, arg3: (boolean)?): string
    function getSystemFontPathForText(self, arg0: string, arg1: string, arg2: (string)?, arg3: (string)?, arg4: (number)?, arg5: (number)?, arg6: (boolean)?): PackedStringArray
    function getExecutablePath(self): string
    function readStringFromStdin(self, arg0: (number)?): string
    function readBufferFromStdin(self, arg0: (number)?): PackedByteArray
    function getStdinType(self): number
    function getStdoutType(self): number
    function getStderrType(self): number
    function execute(self, arg0: string, arg1: PackedStringArray, arg2: (Array)?, arg3: (boolean)?, arg4: (boolean)?): number
    function executeWithPipe(self, arg0: string, arg1: PackedStringArray, arg2: (boolean)?): Dictionary
    function createProcess(self, arg0: string, arg1: PackedStringArray, arg2: (boolean)?): number
    function createInstance(self, arg0: PackedStringArray): number
    function openWithProgram(self, arg0: string, arg1: PackedStringArray): number
    function kill(self, arg0: number): number
    function shellOpen(self, arg0: string): number
    function shellShowInFileManager(self, arg0: string, arg1: (boolean)?): number
    function isProcessRunning(self, arg0: number): boolean
    function getProcessExitCode(self, arg0: number): number
    function getProcessId(self): number
    function hasEnvironment(self, arg0: string): boolean
    function getEnvironment(self, arg0: string): string
    function setEnvironment(self, arg0: string, arg1: string): ()
    function unsetEnvironment(self, arg0: string): ()
    function getName(self): string
    function getDistributionName(self): string
    function getVersion(self): string
    function getVersionAlias(self): string
    function getCmdlineArgs(self): PackedStringArray
    function getCmdlineUserArgs(self): PackedStringArray
    function getVideoAdapterDriverInfo(self): PackedStringArray
    function setRestartOnExit(self, arg0: boolean, arg1: (PackedStringArray)?): ()
    function isRestartOnExitSet(self): boolean
    function getRestartOnExitArguments(self): PackedStringArray
    function delayUsec(self, arg0: number): ()
    function delayMsec(self, arg0: number): ()
    function getLocale(self): string
    function getLocaleLanguage(self): string
    function getModelName(self): string
    function isUserfsPersistent(self): boolean
    function isStdoutVerbose(self): boolean
    function isDebugBuild(self): boolean
    function getStaticMemoryUsage(self): number
    function getStaticMemoryPeakUsage(self): number
    function getMemoryInfo(self): Dictionary
    function moveToTrash(self, arg0: string): number
    function getUserDataDir(self): string
    function getSystemDir(self, arg0: number, arg1: (boolean)?): string
    function getConfigDir(self): string
    function getDataDir(self): string
    function getCacheDir(self): string
    function getTempDir(self): string
    function getUniqueId(self): string
    function getKeycodeString(self, arg0: number): string
    function isKeycodeUnicode(self, arg0: number): boolean
    function findKeycodeFromString(self, arg0: string): number
    function setUseFileAccessSaveAndSwap(self, arg0: boolean): ()
    function setThreadName(self, arg0: string): number
    function getThreadCallerId(self): number
    function getMainThreadId(self): number
    function hasFeature(self, arg0: string): boolean
    function isSandboxed(self): boolean
    function requestPermission(self, arg0: string): boolean
    function requestPermissions(self): boolean
    function getGrantedPermissions(self): PackedStringArray
    function revokeGrantedPermissions(self): ()
    function addLogger(self, arg0: any): ()
    function removeLogger(self, arg0: any): ()
end

declare extern type ProjectSettings extends Object with
    function hasSetting(self, arg0: string): boolean
    function setSetting(self, arg0: string, arg1: any): ()
    function getSetting(self, arg0: string, arg1: (any)?): any
    function getSettingWithOverride(self, arg0: string): any
    function getGlobalClassList(self): Array
    function getSettingWithOverrideAndCustomFeatures(self, arg0: string, arg1: PackedStringArray): any
    function setOrder(self, arg0: string, arg1: number): ()
    function getOrder(self, arg0: string): number
    function setInitialValue(self, arg0: string, arg1: any): ()
    function setAsBasic(self, arg0: string, arg1: boolean): ()
    function setAsInternal(self, arg0: string, arg1: boolean): ()
    function addPropertyInfo(self, arg0: Dictionary): ()
    function setRestartIfChanged(self, arg0: string, arg1: boolean): ()
    function clear(self, arg0: string): ()
    function localizePath(self, arg0: string): string
    function globalizePath(self, arg0: string): string
    function save(self): number
    function loadResourcePack(self, arg0: string, arg1: (boolean)?, arg2: (number)?): boolean
    function saveCustom(self, arg0: string): number
    function getChangedSettings(self): PackedStringArray
    function checkChangedSettingsInGroup(self, arg0: string): boolean
end

declare extern type ResourceLoader extends Object with
    function loadThreadedRequest(self, arg0: string, arg1: (string)?, arg2: (boolean)?, arg3: (number)?): number
    function loadThreadedGetStatus(self, arg0: string, arg1: (Array)?): number
    function loadThreadedGet(self, arg0: string): Resource
    function load(self, arg0: string, arg1: (string)?, arg2: (number)?): Resource
    function getRecognizedExtensionsForType(self, arg0: string): PackedStringArray
    function addResourceFormatLoader(self, arg0: any, arg1: (boolean)?): ()
    function removeResourceFormatLoader(self, arg0: any): ()
    function setAbortOnMissingResources(self, arg0: boolean): ()
    function getDependencies(self, arg0: string): PackedStringArray
    function hasCached(self, arg0: string): boolean
    function getCachedRef(self, arg0: string): Resource
    function exists(self, arg0: string, arg1: (string)?): boolean
    function getResourceUid(self, arg0: string): number
    function listDirectory(self, arg0: string): PackedStringArray
end

declare extern type ResourceSaver extends Object with
    function save(self, arg0: Resource, arg1: (string)?, arg2: (number)?): number
    function setUid(self, arg0: string, arg1: number): number
    function getRecognizedExtensions(self, arg0: Resource): PackedStringArray
    function addResourceFormatSaver(self, arg0: any, arg1: (boolean)?): ()
    function removeResourceFormatSaver(self, arg0: any): ()
    function getResourceIdForPath(self, arg0: string, arg1: (boolean)?): number
end

declare extern type SceneTree extends MainLoop with
    AutoAcceptQuit: boolean
    QuitOnGoBack: boolean
    DebugCollisionsHint: boolean
    DebugPathsHint: boolean
    DebugNavigationHint: boolean
    Paused: boolean
    EditedSceneRoot: Node
    CurrentScene: Node
    Root: Node
    MultiplayerPoll: boolean
    PhysicsInterpolation: boolean
    function getRoot(self): any
    function hasGroup(self, arg0: string): boolean
    function isAccessibilityEnabled(self): boolean
    function isAccessibilitySupported(self): boolean
    function isAutoAcceptQuit(self): boolean
    function setAutoAcceptQuit(self, arg0: boolean): ()
    function isQuitOnGoBack(self): boolean
    function setQuitOnGoBack(self, arg0: boolean): ()
    function setDebugCollisionsHint(self, arg0: boolean): ()
    function isDebuggingCollisionsHint(self): boolean
    function setDebugPathsHint(self, arg0: boolean): ()
    function isDebuggingPathsHint(self): boolean
    function setDebugNavigationHint(self, arg0: boolean): ()
    function isDebuggingNavigationHint(self): boolean
    function setEditedSceneRoot(self, arg0: Node): ()
    function getEditedSceneRoot(self): Node
    function setPause(self, arg0: boolean): ()
    function isPaused(self): boolean
    function createTimer(self, arg0: number, arg1: (boolean)?, arg2: (boolean)?, arg3: (boolean)?): any
    function createTween(self): any
    function getProcessedTweens(self): Array
    function getNodeCount(self): number
    function getFrame(self): number
    function quit(self, arg0: (number)?): ()
    function setPhysicsInterpolationEnabled(self, arg0: boolean): ()
    function isPhysicsInterpolationEnabled(self): boolean
    function queueDelete(self, arg0: Object): ()
    function callGroupFlags(self, arg0: number, arg1: string, arg2: string, ...: any): ()
    function notifyGroupFlags(self, arg0: number, arg1: string, arg2: number): ()
    function setGroupFlags(self, arg0: number, arg1: string, arg2: string, arg3: any): ()
    function callGroup(self, arg0: string, arg1: string, ...: any): ()
    function notifyGroup(self, arg0: string, arg1: number): ()
    function setGroup(self, arg0: string, arg1: string, arg2: any): ()
    function getNodesInGroup(self, arg0: string): Array
    function getFirstNodeInGroup(self, arg0: string): Node
    function getNodeCountInGroup(self, arg0: string): number
    function setCurrentScene(self, arg0: Node): ()
    function getCurrentScene(self): Node
    function changeSceneToFile(self, arg0: string): number
    function changeSceneToPacked(self, arg0: any): number
    function changeSceneToNode(self, arg0: Node): number
    function reloadCurrentScene(self): number
    function unloadCurrentScene(self): ()
    function setMultiplayer(self, arg0: any, arg1: (string)?): ()
    function getMultiplayer(self, arg0: (string)?): any
    function setMultiplayerPollEnabled(self, arg0: boolean): ()
    function isMultiplayerPollEnabled(self): boolean
end

declare extern type Script extends Resource with
    SourceCode: string
    function canInstantiate(self): boolean
    function hasSourceCode(self): boolean
    function getSourceCode(self): string
    function setSourceCode(self, arg0: string): ()
    function reload(self, arg0: (boolean)?): number
    function getBaseScript(self): Script
    function getInstanceBaseType(self): string
    function getGlobalName(self): string
    function hasScriptMethod(self, arg0: string): boolean
    function hasScriptSignal(self, arg0: string): boolean
    function getScriptPropertyList(self): Array
    function getScriptMethodList(self): Array
    function getScriptSignalList(self): Array
    function getScriptConstantMap(self): Dictionary
    function getPropertyDefaultValue(self, arg0: string): any
    function isTool(self): boolean
    function isAbstract(self): boolean
    function getRpcConfig(self): any
    function instanceHas(self, arg0: Object): boolean
end

declare extern type Shader extends Resource with
    Code: string
    function getMode(self): number
    function setCode(self, arg0: string): ()
    function getCode(self): string
    function setDefaultTextureParameter(self, arg0: string, arg1: Texture, arg2: (number)?): ()
    function getDefaultTextureParameter(self, arg0: string, arg1: (number)?): Texture
    function getShaderUniformList(self, arg0: (boolean)?): Array
    function inspectNativeShaderCode(self): ()
end

declare extern type ShaderMaterial extends Material with
    Shader: Shader
    function setShader(self, arg0: Shader): ()
    function getShader(self): Shader
    function setShaderParameter(self, arg0: string, arg1: any): ()
    function getShaderParameter(self, arg0: string): any
end

declare extern type Sprite2D extends Node2D with
    Texture: Texture2D
    Centered: boolean
    Offset: Vector2
    FlipH: boolean
    FlipV: boolean
    Hframes: number
    Vframes: number
    Frame: number
    FrameCoords: Vector2i
    RegionEnabled: boolean
    RegionRect: Rect2
    RegionFilterClipEnabled: boolean
    function setTexture(self, arg0: Texture2D): ()
    function getTexture(self): Texture2D
    function setCentered(self, arg0: boolean): ()
    function isCentered(self): boolean
    function setOffset(self, arg0: Vector2): ()
    function getOffset(self): Vector2
    function setFlipH(self, arg0: boolean): ()
    function isFlippedH(self): boolean
    function setFlipV(self, arg0: boolean): ()
    function isFlippedV(self): boolean
    function setRegionEnabled(self, arg0: boolean): ()
    function isRegionEnabled(self): boolean
    function isPixelOpaque(self, arg0: Vector2): boolean
    function setRegionRect(self, arg0: Rect2): ()
    function getRegionRect(self): Rect2
    function setRegionFilterClipEnabled(self, arg0: boolean): ()
    function isRegionFilterClipEnabled(self): boolean
    function setFrame(self, arg0: number): ()
    function getFrame(self): number
    function setFrameCoords(self, arg0: Vector2i): ()
    function getFrameCoords(self): Vector2i
    function setVframes(self, arg0: number): ()
    function getVframes(self): number
    function setHframes(self, arg0: number): ()
    function getHframes(self): number
    function getRect(self): Rect2
end

declare extern type Texture extends Resource with
end

declare extern type Texture2D extends Texture with
    function _getImage(self): any
    function _getFormat(self): number
    function _getMipmapCount(self): number
    function _getWidth(self): number
    function _getHeight(self): number
    function _isPixelOpaque(self, arg0: number, arg1: number): boolean
    function _hasAlpha(self): boolean
    function _hasMipmaps(self): boolean
    function _draw(self, arg0: RID, arg1: Vector2, arg2: Color, arg3: boolean): ()
    function _drawRect(self, arg0: RID, arg1: Rect2, arg2: boolean, arg3: Color, arg4: boolean): ()
    function _drawRectRegion(self, arg0: RID, arg1: Rect2, arg2: Rect2, arg3: Color, arg4: boolean, arg5: boolean): ()
    function getFormat(self): number
    function getMipmapCount(self): number
    function getWidth(self): number
    function getHeight(self): number
    function getSize(self): Vector2
    function hasAlpha(self): boolean
    function hasMipmaps(self): boolean
    function draw(self, arg0: RID, arg1: Vector2, arg2: (Color)?, arg3: (boolean)?): ()
    function drawRect(self, arg0: RID, arg1: Rect2, arg2: boolean, arg3: (Color)?, arg4: (boolean)?): ()
    function drawRectRegion(self, arg0: RID, arg1: Rect2, arg2: Rect2, arg3: (Color)?, arg4: (boolean)?, arg5: (boolean)?): ()
    function getImage(self): any
    function createPlaceholder(self): Resource
end

declare extern type Time extends Object with
    function getDatetimeDictFromUnixTime(self, arg0: number): Dictionary
    function getDateDictFromUnixTime(self, arg0: number): Dictionary
    function getTimeDictFromUnixTime(self, arg0: number): Dictionary
    function getDatetimeStringFromUnixTime(self, arg0: number, arg1: (boolean)?): string
    function getDateStringFromUnixTime(self, arg0: number): string
    function getTimeStringFromUnixTime(self, arg0: number): string
    function getDatetimeDictFromDatetimeString(self, arg0: string, arg1: boolean): Dictionary
    function getDatetimeStringFromDatetimeDict(self, arg0: Dictionary, arg1: boolean): string
    function getUnixTimeFromDatetimeDict(self, arg0: Dictionary): number
    function getUnixTimeFromDatetimeString(self, arg0: string): number
    function getOffsetStringFromOffsetMinutes(self, arg0: number): string
    function getDatetimeDictFromSystem(self, arg0: (boolean)?): Dictionary
    function getDateDictFromSystem(self, arg0: (boolean)?): Dictionary
    function getTimeDictFromSystem(self, arg0: (boolean)?): Dictionary
    function getDatetimeStringFromSystem(self, arg0: (boolean)?, arg1: (boolean)?): string
    function getDateStringFromSystem(self, arg0: (boolean)?): string
    function getTimeStringFromSystem(self, arg0: (boolean)?): string
    function getTimeZoneFromSystem(self): Dictionary
    function getUnixTimeFromSystem(self): number
    function getTicksMsec(self): number
    function getTicksUsec(self): number
end

declare Engine: Engine
declare ProjectSettings: ProjectSettings
declare OS: OS
declare Time: Time
declare ResourceLoader: ResourceLoader
declare ResourceSaver: ResourceSaver
declare Input: Input
declare InputMap: InputMap
