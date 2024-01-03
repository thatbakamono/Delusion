using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DelusionSharp
{
    internal static unsafe class Internals
    {
        // Input
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void IsKeyDown(Key key, bool* result);
        
        // Transform component
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void HasTransformComponent(UniqueId id, bool* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetTransformPosition(UniqueId id, Vector2* result);
   
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformPosition(UniqueId id, Vector2 position);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetTransformScale(UniqueId id, Vector2* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformScale(UniqueId id, Vector2 scale);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetTransformRotation(UniqueId id, float* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransformRotation(UniqueId id, float rotation);
        
        // Sprite component
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void HasSpriteComponent(UniqueId id, bool* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetSpriteTexture(UniqueId id, UniqueId* result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetSpriteTexture(UniqueId entityId, UniqueId textureId);

        // Rigidbody component

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void HasRigidbodyComponent(UniqueId id, bool* result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRigidbodyBodyType(UniqueId id, BodyType* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyBodyType(UniqueId id, BodyType bodyType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRigidbodyHasFixedRotation(UniqueId id, bool* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyHasFixedRotation(UniqueId id, bool hasFixedRotation);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRigidbodyDensity(UniqueId id, float* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyDensity(UniqueId id, float density);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRigidbodyFriction(UniqueId id, float* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyFriction(UniqueId id, float friction);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRigidbodyRestitution(UniqueId id, float* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyRestitution(UniqueId id, float restitution);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRigidbodyRestitutionThreshold(UniqueId id, float* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRigidbodyRestitutionThreshold(UniqueId id, float restitutionThreshold);
        
        // Box collider component
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void HasBoxColliderComponent(UniqueId id, bool* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetBoxColliderSize(UniqueId id, Vector2* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetBoxColliderSize(UniqueId id, Vector2 size);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetBoxColliderOffset(UniqueId id, Vector2* result);
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetBoxColliderOffset(UniqueId id, Vector2 offset);
    }
}
