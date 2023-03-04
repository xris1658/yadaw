# Conventions of defining and implementing Models used in YADAW

## Defining Models
- Derive your Model from QAbstractListModel.
- Provide a constructor, virtual destructor and a public `static constexpr int columnCount()`.

## Naming convention
- Class names: `***ListModel` or `***Model` in PascalCase
- Roles: PascalCase
- Role names: `***_xxx` in snake_case, in which
  - `***` is the upper case letters of the class name, in lower case.
  - `xxx` is the role in lower case.

For example, class `AssetDirectoryListModel` has its roles and role names as follows:
- `Id` named `adlm_id`
- `Path` named `adlm_path`
- `Name` named `adlm_name`

## Roles
- Define roles using nested unscoped `enum`.
  - Always assign the first enumerator with `Qt::UserRole`.
  - Always end the enumerators with `RoleCount`.
  - With roles defined as such, add a member function
    ```cpp
    public:
        static constexpr int roleCount() { return RoleCount - Qt::UserRole; }
    ```

## Some member functions
Implement `columnCount()` and `columnCount(const QModelIndex&)` as follows:
```cpp
public:
    static constexpr int columnCount() { return 1; }
    int columnCount(const QModelIndex&) const override final { rturn columnCount(); }
```