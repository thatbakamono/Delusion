namespace DelusionSharp
{
    public class Transform
    {
        private Entity _entity;
        
        public Vector2 Position
        {
            get => Internals.GetTransformPosition(_entity._id);
            set => Internals.SetTransformPosition(_entity._id, value);
        }
        
        public Vector2 Scale
        {
            get => Internals.GetTransformScale(_entity._id);
            set => Internals.SetTransformScale(_entity._id, value);
        }
        
        public float Rotation
        {
            get => Internals.GetTransformRotation(_entity._id);
            set => Internals.SetTransformRotation(_entity._id, value);
        }
        
        public Transform(Entity entity)
        {
            _entity = entity;
        }
    }
}
