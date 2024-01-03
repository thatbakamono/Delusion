namespace DelusionSharp
{
    public unsafe class BoxCollider
    {
        private Entity _entity;

        public Vector2 Size
        {
            get
            {
                Vector2 result;
                
                Internals.GetBoxColliderSize(_entity._id, &result);

                return result;
            }
            set => Internals.SetBoxColliderSize(_entity._id, value);
        }

        public Vector2 Offset
        {
            get
            {
                Vector2 result;
                
                Internals.GetBoxColliderOffset(_entity._id, &result);

                return result;
            }
            set => Internals.SetBoxColliderOffset(_entity._id, value);
        }
        
        public BoxCollider(Entity entity)
        {
            _entity = entity;
        }
    }
}
