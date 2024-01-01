using System.Runtime.CompilerServices;

namespace DelusionSharp
{
    internal static class Internals
    {
        // Input
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool IsKeyDown(Key key);
        
        // Transform component
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool HasTransformComponent(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Vector2 GetTransformPosition(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformPosition(UniqueId id, Vector2 position);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Vector2 GetTransformScale(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformScale(UniqueId id, Vector2 scale);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetTransformRotation(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformRotation(UniqueId id, float rotation);
        
        // Sprite component
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool HasSpriteComponent(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern UniqueId GetSpriteTexture(UniqueId id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetSpriteTexture(UniqueId entityId, UniqueId textureId);

        // Rigidbody component

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool HasRigidbodyComponent(UniqueId id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern BodyType GetRigidbodyBodyType(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyBodyType(UniqueId id, BodyType bodyType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetRigidbodyHasFixedRotation(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyHasFixedRotation(UniqueId id, bool hasFixedRotation);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRigidbodyDensity(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyDensity(UniqueId id, float density);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRigidbodyFriction(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyFriction(UniqueId id, float friction);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRigidbodyRestitution(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyRestitution(UniqueId id, float restitution);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetRigidbodyRestitutionThreshold(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyRestitutionThreshold(UniqueId id, float restitutionThreshold);
        
        // Box collider component
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool HasBoxColliderComponent(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Vector2 GetBoxColliderSize(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetBoxColliderSize(UniqueId id, Vector2 size);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Vector2 GetBoxColliderOffset(UniqueId id);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetBoxColliderOffset(UniqueId id, Vector2 offset);
    }
}
