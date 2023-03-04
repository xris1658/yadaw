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
- Define roles using nested `enum`.
  - Always assign the first enumerator with `Qt::UserRole`.
  - Always end the enumerators with `RoleCount`.

With roles defined like this, `columnCount()` and `columnCount(const QModelIndex&)` can be implemented as follows:
```cpp
public:
    static constexpr int columnCount() { return RoleCount - Qt::UserRole; }
    int columnCount(const QModelIndex&) const override final { rturn columnCount(); }
```