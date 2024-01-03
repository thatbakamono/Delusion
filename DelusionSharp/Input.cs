namespace DelusionSharp
{
    public unsafe class Input
    {
        public static bool IsKeyDown(Key key)
        {
            bool result;
            
            Internals.IsKeyDown(key, &result);

            return result;
        }
    }
}
