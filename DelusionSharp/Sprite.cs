namespace DelusionSharp
{
    public unsafe class Sprite
    {
        private Entity _entity;
        
        public Texture2D Texture
        {
            get
            {
                UniqueId result;
                
                Internals.GetSpriteTexture(_entity._id, &result);

                return new Texture2D(result);
            }
            set => Internals.SetSpriteTexture(_entity._id, value._id);
        }

        public Sprite(Entity entity)
        {
            _entity = entity;
        }
    }
}
