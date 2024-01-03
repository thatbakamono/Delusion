using System;

namespace DelusionSharp
{
    public unsafe class Entity
    {
        internal UniqueId _id;

        public bool HasComponent<T>()
        {
            var type = typeof(T);
            
            bool result = false;

            if (type == typeof(Transform))
            {
                Internals.HasTransformComponent(_id, &result);
                
                return result;
            }

            if (type == typeof(Sprite))
            {
                Internals.HasSpriteComponent(_id, &result);
                
                return result;
            }
            
            if (type == typeof(Rigidbody))
            {
                Internals.HasRigidbodyComponent(_id, &result);
                
                return result;
            }
            
            if (type == typeof(BoxCollider))
            {
                Internals.HasBoxColliderComponent(_id, &result);
                
                return result;
            }
            
            throw new Exception("Invalid component");
        }
        
        public T GetComponent<T>()
        {
            var type = typeof(T);
            
            if (type == typeof(Transform))
            {
                if (HasComponent<Transform>())
                    return (T) (object) new Transform(this);
                
                throw new Exception("Entity does not have transform component");
            }

            if (type == typeof(Sprite))
            {
                if (HasComponent<Sprite>())
                    return (T) (object) new Sprite(this);
                
                throw new Exception("Entity does not have sprite component");
            }

            if (type == typeof(Rigidbody))
            {
                if (HasComponent<Rigidbody>())
                    return (T) (object) new Rigidbody(this);
                
                throw new Exception("Entity does not have rigidbody component");
            }

            if (type == typeof(BoxCollider))
            {
                if (HasComponent<BoxCollider>())
                    return (T) (object) new BoxCollider(this);
                
                throw new Exception("Entity does not have box collider component");
            }

            throw new Exception("Invalid component");
        }
    }
}
