tree >src.txt
{
    printf '# Reimann Playground @ %s\n' "$(git log -1 --format='%h %s (%ci)')"
    printf '# files: %d\n\n' "$(git ls-files -- ':!LICENSE' | wc -l)"
    git ls-files -z -- ':!LICENSE' ':!stb_image_write.h' | while IFS= read -r -d '' f; do
        printf '=== %s ===\n' "$f"
        cat "$f"
        echo
    done
} >>src.txt

cat src.txt | wl-copy
