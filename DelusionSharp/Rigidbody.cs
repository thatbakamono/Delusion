namespace DelusionSharp
{
    public unsafe class Rigidbody
    {
        private Entity _entity;

        public BodyType BodyType
        {
            get
            {
                BodyType result;
                
                Internals.GetRigidbodyBodyType(_entity._id, &result);

                return result;
            }
            set => Internals.SetRigidbodyBodyType(_entity._id, value);
        }
        
        public bool HasFixedRotation
        {
            get
            {
                bool result;
                
                Internals.GetRigidbodyHasFixedRotation(_entity._id, &result);

                return result;
            }
            set => Internals.SetRigidbodyHasFixedRotation(_entity._id, value);
        }
        
        public float Density
        {
            get
            {
                float result;
                
                Internals.GetRigidbodyDensity(_entity._id, &result);

                return result;
            }
            set => Internals.SetRigidbodyDensity(_entity._id, value);
        }
        
        public float Friction
        {
            get
            {
                float result;
                
                Internals.GetRigidbodyFriction(_entity._id, &result);

                return result;
            }
            set => Internals.SetRigidbodyFriction(_entity._id, value);
        }
        
        public float Restitution
        {
            get
            {
                float result;
                
                Internals.GetRigidbodyRestitution(_entity._id, &result);

                return result;
            }
            set => Internals.SetRigidbodyRestitution(_entity._id, value);
        }
        
        public float RestitutionThreshold
        {
            get
            {
                float result;
                
                Internals.GetRigidbodyRestitutionThreshold(_entity._id, &result);

                return result;
            }
            set => Internals.SetRigidbodyRestitutionThreshold(_entity._id, value);
        }
        
        public Rigidbody(Entity entity)
        {
            _entity = entity;
        }
    }
}
