Scribe, your lovable personal assistant.
---

Scribe is a simple C++ todo Command Line Application (CLI).

Usage:
  scribe add TODO         - Creates TODO as a new todo item.
  scribe ls [--arc]       - Lists all todos.
  scribe ls @TAG [--arc]  - Lists all todos with TAG.
  scribe rm [--arc] ID    - Permanently deletes todo with ID.
  scribe done ID          - Marks as completed todo with ID and archives.
  scribe done @TAG        - Marks as completed all todos with TAG and archives.
  scribe clear [--rm]     - Marks as completed all todos.
  scribe undo ID          - Moves todo with ID in archive file to todo file.

Options
  --arc  - Perform on Archive File.
  --rm   - Permanently delete, skip archiving.