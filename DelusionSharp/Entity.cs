using System;

namespace DelusionSharp
{
    public class Entity
    {
        internal UniqueId _id;

        public T GetComponent<T>()
        {
            var type = typeof(T);
            
            if (type == typeof(Transform))
            {
                if (Internals.HasTransformComponent(_id))
                    return (T) (object) new Transform(this);
                
                throw new Exception("Entity does not have transform component");
            }

            if (type == typeof(Rigidbody))
            {
                if (Internals.HasRigidbodyComponent(_id))
                    return (T) (object) new Rigidbody(this);
                
                throw new Exception("Entity does not have rigidbody component");
            }

            if (type == typeof(BoxCollider))
            {
                if (Internals.HasBoxColliderComponent(_id))
                    return (T) (object) new BoxCollider(this);
                
                throw new Exception("Entity does not have box collider component");
            }

            throw new Exception("Invalid component");
        }
    }
}
