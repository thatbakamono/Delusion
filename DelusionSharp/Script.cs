namespace DelusionSharp
{
    public abstract class Script
    {
        protected Entity Entity
        {
            get => _entity;
        }

        private Entity _entity;
        
        public abstract void OnCreate();
        public abstract void OnUpdate(float deltaTime);
    }
}
