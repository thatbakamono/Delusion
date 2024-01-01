namespace DelusionSharp
{
    public class Sprite
    {
        private Entity _entity;
        
        public Texture2D Texture
        {
            get => new Texture2D(Internals.GetSpriteTexture(_entity._id));
            set => Internals.SetSpriteTexture(_entity._id, value._id);
        }

        public Sprite(Entity entity)
        {
            _entity = entity;
        }
    }
}
