namespace DelusionSharp
{
    public struct UniqueId
    {
        private readonly ulong _id;
        
        public UniqueId(ulong id)
        {
            _id = id;
        }
    }
}
