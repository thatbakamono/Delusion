namespace DelusionSharp
{
    public unsafe class Transform
    {
        private Entity _entity;
        
        public Vector2 Position
        {
            get
            {
                Vector2 result;
                
                Internals.GetTransformPosition(_entity._id, &result);
                
                return result;
            }
            set => Internals.SetTransformPosition(_entity._id, value);
        }
        
        public Vector2 Scale
        {
            get
            {
                Vector2 result;

                Internals.GetTransformScale(_entity._id, &result);
                
                return result;
            }
            set => Internals.SetTransformScale(_entity._id, value);
        }
        
        public float Rotation
        {
            get
            {
                float result;

                Internals.GetTransformRotation(_entity._id, &result);
                
                return result;
            }
            set => Internals.SetTransformRotation(_entity._id, value);
        }
        
        public Transform(Entity entity)
        {
            _entity = entity;
        }
    }
}
