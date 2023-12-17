namespace DelusionSharp
{
    public class Rigidbody
    {
        private Entity _entity;

        public BodyType BodyType
        {
            get => Internals.GetRigidbodyBodyType(_entity._id);
            set => Internals.SetRigidbodyBodyType(_entity._id, value);
        }
        
        public bool HasFixedRotation
        {
            get => Internals.GetRigidbodyHasFixedRotation(_entity._id);
            set => Internals.SetRigidbodyHasFixedRotation(_entity._id, value);
        }
        
        public float Density
        {
            get => Internals.GetRigidbodyDensity(_entity._id);
            set => Internals.SetRigidbodyDensity(_entity._id, value);
        }
        
        public float Friction
        {
            get => Internals.GetRigidbodyFriction(_entity._id);
            set => Internals.SetRigidbodyFriction(_entity._id, value);
        }
        
        public float Restitution
        {
            get => Internals.GetRigidbodyRestitution(_entity._id);
            set => Internals.SetRigidbodyRestitution(_entity._id, value);
        }
        
        public float RestitutionThreshold
        {
            get => Internals.GetRigidbodyRestitutionThreshold(_entity._id);
            set => Internals.SetRigidbodyRestitutionThreshold(_entity._id, value);
        }
        
        public Rigidbody(Entity entity)
        {
            _entity = entity;
        }
    }
}
