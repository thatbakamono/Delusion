namespace DelusionSharp
{
    public class BoxCollider
    {
        private Entity _entity;

        public Vector2 Size
        {
            get => Internals.GetBoxColliderSize(_entity._id);
            set => Internals.SetBoxColliderSize(_entity._id, value);
        }

        public Vector2 Offset
        {
            get => Internals.GetBoxColliderOffset(_entity._id);
            set => Internals.SetBoxColliderOffset(_entity._id, value);
        }
        
        public BoxCollider(Entity entity)
        {
            _entity = entity;
        }
    }
}
